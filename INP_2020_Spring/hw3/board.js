const Board = require("./db.js").Board;
const Post = require("./db.js").Post;
const User = require("./db.js").User;

function argumentParser(socket, cmd, data, position, usage) {
    // split raw input into tokens
    let splittedInput = String(data).replace(/\r|\n|\t/g, " ").split(" ").filter((value, index, arr) => {
        if (value != "")
            return value
    });
    // should include all of the positional parameters
    for (let p of position) {
        if (!splittedInput.includes(p)) {
            socket.write(usage);
            return;
        }
    }
    // each positionnal parameters should only appear once
    let appear = [];
    for (let p of position) {
        let count = 0;
        for (let i = 0; i < splittedInput.length; i++) {
            if (splittedInput[i] === p) {
                count++;
                if (count > 1) {
                    socket.write(usage);
                    return;
                } else {
                    appear.push(i);
                }
            }
        }
    }
    // positinal parameters should be held in order
    if (appear.length > 1) {
        for (let i = 1; i < appear.length; i++) {
            if (appear[i] < appear[i - 1]) {
                socket.write(usage);
                return;
            }
        }
    }
    // separate command
    let parsedCommand = [];
    let command = String(data).substring(cmd.length).replace(/\r/g, "").replace(/^\s+/, "");
    // split by first position arg
    let firstSplit = command.split(position[0]);
    if (position.length > 1) {
        parsedCommand.push(firstSplit[0].trim());
        let secondSplit = firstSplit[1].split(position[1]);
        for (let s of secondSplit) {
            // remove leading space of separated strings
            s = s.replace(/^\s+/, "").trim().replace(/<br>/g, "\n");
            parsedCommand.push(s);
        }
    } else {
        for (let c of firstSplit) {
            parsedCommand.push(c.trim().replace(/<br>/g, "\n"));
        }
    }

    // final check, first element(for query) should not be empty
    if (parsedCommand == null || parsedCommand.length === 0 || (parsedCommand.length > 1 && parsedCommand[0] === "")) {
        socket.write(usage);
        return;
    }
    return parsedCommand;
}

module.exports = {
    create_board: function (socket, recv) {
        // check if user had already logged in
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            // check if board name is already used
            let boardName = recv[1];
            let moderator = socket.Session.name;

            Board.countDocuments({ BoardName: boardName }, (err, count) => {
                if (err) throw err;
                // board name exists
                if (count > 0) {
                    socket.write("Board already exist.\n% ");
                    return;
                } else {
                    new Board({
                        BoardName: boardName,
                        Mod: moderator,
                    }).save((err) => {
                        if (err) {
                            console.error("Create Board failed; check mongodb");
                            return;
                        } else {
                            socket.write("Create board successfully.\n% ");
                            return;
                        }
                    });
                }
            });
        }
        return;
    },

    create_post: function (socket, recv, data) {
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            let usage = "Usage: create-post <board-name> --title <title> --content <content>\n% ";
            let parsedInput = argumentParser(socket, recv[0], data, ["--title", "--content"], usage);
            if (parsedInput == null) return;

            let author = socket.Session.name;
            let board = parsedInput[0];
            let title = parsedInput[1];
            let content = parsedInput[2];
            // create post key for s3
            let key = (title.replace(/\s+/g, "-") + "-" + Date.now());
            Board.countDocuments({ BoardName: board }, (err, count) => {
                if (err) throw err;
                if (count > 0) {
                    // to save content to s3, need to pass post content to the client
                    User.findOne({ Username: author }, (err, user) => {
                        if (err) throw err;
                        // save metadata to db
                        new Post({
                            Board: board,
                            Author: author,
                            Title: title,
                            Bucket: user.Bucket,
                            ContentKey: key,
                            CreatedAt: new Date(),
                        }).save((err) => {
                            if (err) {
                                console.error("Create post failed. Check mongoDB:");
                                return;
                            } else {
                                // save metadata succeed, ask client to upload post to s3
                                let s3settings = {
                                    Type: "create-post",
                                    Bucket: user.Bucket,
                                    Key: key,
                                    Content: content
                                };
                                socket.write(JSON.stringify(s3settings));
                                return;
                            }
                        });
                    });
                } else {
                    socket.write("Board does not exist.\n% ");
                    return;
                }
            });
        }
        return;
    },

    list_board: function (socket, recv) {
        if (recv.length === 1) {
            // list all boards
            socket.write("Index\tName\tModerator\n");
            Board.find({}, (err, boards) => {
                if (!err) {
                    for (let b of boards.sort((x, y) => {
                        return (x.BoardIndex > y.BoardIndex) ? 1 : -1;
                    })) {
                        socket.write(b["BoardIndex"] + "\t" + b["BoardName"] + "\t" + b["Mod"] + "\n");
                    }
                    socket.write("% ");
                    return;
                } else {
                    throw err;
                }
            });
        } else {
            // check if second parameter starts with "##"
            let query = recv[1];
            if (query.length > 2 && query.substring(0, 2) === "##") {
                let key = query.substring(2);
                socket.write("Index\tName\tModerator\n");
                Board.find({ BoardName: { $regex: key } }, (err, boards) => {
                    if (!err) {
                        for (let b of boards.sort((x, y) => {
                            return (x.BoardIndex > y.BoardIndex) ? 1 : -1;
                        })) {
                            socket.write(b["BoardIndex"] + "\t" + b["BoardName"] + "\t" + b["Mod"] + "\n");
                        }
                        socket.write("% ");
                        return;
                    } else {
                        throw err;
                    }
                });
            } else {
                socket.write("Usage: list-board ##<key>\n% ");
                return;
            }
        }
        return;
    },

    list_post: function (socket, recv) {
        // check if board exists
        Board.countDocuments({ BoardName: recv[1] }, (err, count) => {
            if (err) throw err;
            if (count === 0) {
                socket.write("Board does not exist.\n% ");
                return;
            } else {
                if (recv.length === 2) {
                    // list all posts
                    socket.write("ID\tTitle\tAuthor\tDate\n");
                    Post.find({ Board: recv[1] }, (err, posts) => {
                        if (!err) {
                            for (let b of posts.sort((x, y) => {
                                return (x.PostIndex > y.PostIndex) ? 1 : -1;
                            })) {
                                let date = b["CreatedAt"];
                                // js month starts with 0
                                let month = (date.getMonth() < 9 ? '0' : '') + (date.getMonth() + 1);
                                let day = (date.getDate() < 10 ? '0' : '') + date.getDate();
                                socket.write(b["PostIndex"] + "\t" + b["Title"] + "\t" + b["Author"] + "\t" + month + "/" + day + "\n");
                            }
                            socket.write("% ");
                            return;
                        } else {
                            throw err;
                        }
                    });
                } else if (recv.length === 3 && recv[2].substring(0, 2) === "##") {
                    // do advanced query
                    socket.write("ID\tTitle\tAuthor\tDate\n");
                    let key = recv[2].substring(2);
                    let boardName = recv[1];
                    Post.find({ Title: { $regex: key }, Board: boardName }, (err, posts) => {
                        if (!err) {
                            for (let b of posts.sort((x, y) => {
                                return (x.PostIndex > y.PostIndex) ? 1 : -1;
                            })) {
                                let date = b["CreatedAt"];
                                let month = (date.getMonth() < 9 ? '0' : '') + (date.getMonth() + 1);
                                let day = (date.getDate() < 10 ? '0' : '') + date.getDate();
                                socket.write(b["PostIndex"] + "\t" + b["Title"] + "\t" + b["Author"] + "\t" + month + "/" + day + "\n");
                            }
                            socket.write("% ");
                            return;
                        } else {
                            throw err;
                        }
                    })
                } else {
                    socket.write("Usage: list-post <board-name> ##<key>\n% ");
                    return;
                }
            }
        });
        return;
    },

    read: function (socket, recv) {
        let postid = recv[1];
        if (isNaN(postid)) {
            socket.write("Post does not exist.\n% ");
            return;
        }
        // find post's metadata in local db
        Post.findOne({ PostIndex: postid }, (err, post) => {
            if (err) throw err;
            if (post == null) {
                socket.write("Post does not exist.\n% ");
                return;
            } else {
                // send metadata to client
                let author = post["Author"];
                let year = post["CreatedAt"].getYear() + 1900;
                let month = (post["CreatedAt"].getMonth() < 9 ? '0' : '') + (post["CreatedAt"].getMonth() + 1);
                let day = (post["CreatedAt"].getDate() < 10 ? '0' : '') + post["CreatedAt"].getDate();
                let date = year + "-" + month + "-" + day;
                // send post metadat and s3 key to client
                User.findOne({ Username: author }, (err, user) => {
                    if (err) throw err;
                    let s3settings = {
                        Type: "read",
                        Bucket: user.Bucket,
                        Key: post.ContentKey,
                        Author: author,
                        Title: post["Title"],
                        Date: date
                    };
                    socket.write(JSON.stringify(s3settings));
                });
            }
        });
        return;
    },

    delete_post: function (socket, recv) {
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            let user = socket.Session.name;
            let postid = recv[1];
            if (isNaN(postid)) {
                socket.write("Post does not exist.\n% ");
                return;
            }
            Post.findOne({ PostIndex: postid }, (err, post) => {
                if (err) throw err;
                if (post == null) {
                    socket.write("Post does not exist.\n% ");
                    return;
                } else if (post["Author"] !== user) {
                    socket.write("Not the post owner.\n% ");
                    return;
                } else {
                    // post found, delete it from the local db
                    let key = post["ContentKey"];
                    let bucket = post["Bucket"];
                    Post.findOneAndDelete({ PostIndex: postid }, (err) => {
                        if (err) throw err;
                        else {
                            // tell client to delete post from s3
                            let deleteData = {
                                Type: "delete-post",
                                Bucket: bucket,
                                Key: key
                            };
                            socket.write(JSON.stringify(deleteData));
                            return;
                        }
                    });
                }
            });
        }
        return;
    },

    update_post: function (socket, recv, data) {
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            // check if user intends to change title or post content
            let usage = "Usage: update-post <post-id> --title/content <new>\n% ";
            let typeOfUpdate = undefined;
            let newInput = undefined;
            if (recv.includes("--title") && recv.includes("--content")) {
                socket.write(usage);
                return;
            } else if (recv.includes("--title") && !recv.includes("--content")) {
                // update title
                let parsedInput = argumentParser(socket, recv[0], data, ["--title"], usage);
                if (parsedInput == null) return;
                newInput = parsedInput[1];
                typeOfUpdate = "Title";
            } else if (recv.includes("--content") && !recv.includes("--title")) {
                // update content
                let parsedInput = argumentParser(socket, recv[0], data, ["--content"], usage)
                if (parsedInput == null) return;
                newInput = parsedInput[1];
                typeOfUpdate = "Content";
            } else {
                socket.write(usage);
                return;
            }
            let user = socket.Session.name;
            let postid = recv[1];
            if (isNaN(postid)) {
                socket.write("Post does not exist.\n% ");
                return;
            } else {
                Post.findOne({ PostIndex: postid }, (err, post) => {
                    // check if user is the post owner
                    if (err) throw err;
                    if (post == null) {
                        socket.write("Post does not exist.\n% ");
                        return;
                    }
                    if (post["Author"] !== user) {
                        socket.write("Not the post owner.\n% ");
                        return;
                    } else {
                        let update = new Object();
                        switch (typeOfUpdate) {
                            case "Title":
                                update = { Type: "title", Title: newInput };
                                break;
                            case "Content":
                                update = { Type: "content", Content: newInput };
                                break;
                            default: // should not be in here
                                break;
                        }
                        switch (update["Type"]) {
                            case "title": {// update local db
                                let dbUpdate = { "Title": update["Title"] };
                                Post.updateOne({ PostIndex: postid }, { "$set": dbUpdate }, (err, success) => {
                                    if (err) throw err;
                                    if (success) socket.write("Update successfully.\n% ")
                                    else console.error("Update failed. check mongodb:");
                                    return;
                                });
                            }
                                break;
                            case "content": { // update s3
                                // find post key
                                let s3settings = {
                                    Type: "update-post",
                                    Bucket: post["Bucket"],
                                    Key: post["ContentKey"],
                                    Content: update["Content"],
                                };
                                socket.write(JSON.stringify(s3settings));
                            }
                                break;
                        }

                    }
                });
            }
        }
        return;
    },

    comment: function (socket, recv, data) {
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            let postid = recv[1];
            // first parameter should be numeric
            if (isNaN(postid)) {
                socket.write("Post does not exist.\n% ");
                return;
            }
            // get comment
            let commentContent = String(data).substring(String(data).indexOf(postid) + postid.length).trim();

            // find post ID
            Post.findOne({ PostIndex: postid }, (err, post) => {
                if (err) throw err;
                if (post == null) {
                    socket.write("Post does not exist.\n% ");
                    return;
                } else {
                    let newComment = {
                        Type: "comment-post",
                        Bucket: post["Bucket"],
                        Key: post["ContentKey"],
                        Commentator: socket.Session.name,
                        Comment: commentContent,
                    };
                    socket.write(JSON.stringify(newComment));
                }
            });
        }
        return;
    },
};
