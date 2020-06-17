const Board = require("./db.js").Board;
const Post = require("./db.js").Post;

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
                if (err) {
                    throw err;
                }
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
            let author = socket.Session.name;
            let usage = "Usage: create-post <board-name> --title <title> --content <content>\n% ";
            let parsedCmd = argumentParser(socket, recv[0], data, ["--title", "--content"], usage);
            if (parsedCmd == null) return;
            let board = parsedCmd[0];
            let title = parsedCmd[1];
            let content = parsedCmd[2];
            Board.countDocuments({ BoardName: board }, (err, count) => {
                if (err) throw err;
                if (count > 0) {
                    new Post({
                        Board: board,
                        Author: author,
                        Title: title,
                        Content: content,
                        CreatedAt: new Date(),
                        Comments: []
                    }).save((err) => {
                        if (err) {
                            console.error("Create post failed. Check mongoDB:");
                            return;
                        } else {
                            socket.write("Create post successfully.\n% ");
                            return;
                        }
                    });
                } else {
                    // count == 0
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
        Post.countDocuments({ PostIndex: postid }, (err, count) => {
            if (err) throw err;
            if (count > 0) {
                Post.findOne({ PostIndex: postid }, (err, post) => {
                    if (err) throw err;
                    socket.write("Author\t:" + post["Author"] + "\n");
                    socket.write("Title\t:" + post["Title"] + "\n");
                    let year = post["CreatedAt"].getYear() + 1900;
                    let month = (post["CreatedAt"].getMonth() < 9 ? '0' : '') + (post["CreatedAt"].getMonth() + 1);
                    let day = (post["CreatedAt"].getDate() < 10 ? '0' : '') + post["CreatedAt"].getDate();
                    socket.write("Date\t:" + year + "-" + month + "-" + day + "\n");
                    socket.write("--\n" + post["Content"] + "\n--\n");
                    // print comments
                    for (let comment of post.Comments) {
                        socket.write(comment["Commentator"] + ": " + comment["Comment"] + "\n");
                    }
                    socket.write("% ");
                });
            } else {
                socket.write("Post does not exist.\n% ");
                return;
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
                    Post.findOneAndDelete({ PostIndex: postid }, (err) => {
                        if (err) throw err;
                        else {
                            socket.write("Delete successfully.\n% ");
                            return;
                        }
                    })
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
            // check if user types --title or --content
            let usage = "Usage: update-post <post-id> --title/content <new>\n% ";
            let typeOfUpdate = undefined;
            let newInput = undefined;
            if (recv.includes("--title") && recv.includes("--content")) {
                socket.write(usage);
                return;
            } else if (recv.includes("--title") && !recv.includes("--content")) {
                let parsedCmd = argumentParser(socket, recv[0], data, ["--title"], usage);
                if (parsedCmd == null) return;
                newInput = parsedCmd[1];
                typeOfUpdate = "Title";
            } else if (recv.includes("--content") && !recv.includes("--title")) {
                let parsedCmd = argumentParser(socket, recv[0], data, ["--content"], usage)
                if (parsedCmd == null) return;
                newInput = parsedCmd[1];
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
                                update = { "Title": newInput };
                                break;
                            case "Content":
                                update = { "Content": newInput };
                                break;
                            default: // should not be in here
                                break;
                        }
                        Post.updateOne({ PostIndex: postid }, { "$set": update }, (err, success) => {
                            if (err) throw err;
                            if (success) socket.write("Update successfully.\n% ")
                            else console.error("Update failed. check mongodb:");
                            return;
                        });
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
            let newComment = new Object();
            newComment.Commentator = socket.Session.name;
            newComment.Comment = commentContent;

            Post.findOneAndUpdate({ PostIndex: postid }, { "$push": { Comments: newComment } }, (err, success) => {
                if (err) throw err;
                if (success == null) {
                    socket.write("Post does not exist.\n% ");
                    return;
                } else {
                    socket.write("Comment successfully.\n% ");
                    return;
                }
            });
        }
        return;
    },
};
