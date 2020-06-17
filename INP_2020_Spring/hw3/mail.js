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
    mail_to: function (socket, recv, data) {
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            let usage = "Usage: mail-to <username> --subject <subject> --content <content>\n% ";
            let parsedInput = argumentParser(socket, recv[0], data, ["--subject", "--content"], usage);
            if (parsedInput == null) return;

            let sender = socket.Session.name;
            let recipient = parsedInput[0];
            let subject = parsedInput[1];
            let key = ("mail-" + subject.replace(/\s+/g, "-")) + "-" + Date.now();
            let content = parsedInput[2];

            // find recipient and save metadata to the user
            User.findOne({ Username: recipient }, (err, user) => {
                if (err) throw err;
                if (user == null) {
                    socket.write(recipient + " does not exist.\n% ");
                    return;
                } else {
                    let newMail = {
                        "From": sender,
                        "Subject": subject,
                        "Date": new Date(),
                        "Key": key,
                    };
                    // push to user's mail array
                    User.updateOne({ Username: recipient }, { "$push": { "Mail": newMail } }, (err, success) => {
                        if (err) throw err;
                        if (success) {
                            // let client upload content to the recipient's bucket
                            let s3settings = {
                                Type: "mail-to",
                                Bucket: user["Bucket"],
                                Key: key,
                                Content: content,
                            };
                            socket.write(JSON.stringify(s3settings));
                        }
                        else console.error("Mail-to failed. check mongodb:");
                        return;
                    });
                }
            });
        }
        return;
    },

    list_mail: function (socket) {
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            let user = socket.Session.name;
            User.findOne({ Username: user }, (err, user) => {
                if (err) throw err;
                if (user == null) socket.write("Please login first.\n% ");
                else {
                    socket.write("ID\tSubject\tFrom\tDate\n");
                    let mailID = 1;
                    for (let mail of user["Mail"]) {
                        let date = mail["Date"];
                        let month = (date.getMonth() < 9 ? '0' : '') + (date.getMonth() + 1);
                        let day = (date.getDate() < 10 ? '0' : '') + date.getDate();
                        socket.write(mailID + "\t" + mail["Subject"] + "\t" + mail["From"] + "\t" + month + "/" + day + "\n");
                        mailID++;
                    }
                    socket.write("% ");
                    return;
                }
            });
        }
    },
    retr_mail: function (socket, recv) {
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            let username = socket.Session.name;
            User.findOne({ Username: username }, (err, user) => {
                if (err) throw err;
                if (user == null) socket.write("Please login first.\n% ");
                else {
                    if (isNaN(recv[1]) || user["Mail"].length < recv[1] || recv[1] <= 0) {
                        socket.write("No such mail.\n% ");
                        return;
                    }
                    let mailID = 1;
                    for (let mail of user["Mail"]) {
                        if (mailID == recv[1]) {
                            let year = mail["Date"].getYear() + 1900;
                            let month = (mail["Date"].getMonth() < 9 ? '0' : '') + (mail["Date"].getMonth() + 1);
                            let day = (mail["Date"].getDate() < 10 ? '0' : '') + mail["Date"].getDate();
                            let date = year + "-" + month + "-" + day;
                            let mailInfo = {
                                Type: "retr-mail",
                                Key: mail["Key"],
                                Subject: mail["Subject"],
                                From: mail["From"],
                                Date: date,
                                Bucket: user["Bucket"],
                            };
                            socket.write(JSON.stringify(mailInfo));
                            return;
                        }
                        mailID++;
                    }
                }
            });
        }
    },
    delete_mail: function (socket, recv) {
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
            return;
        } else {
            let username = socket.Session.name;
            User.findOne({ Username: username }, (err, user) => {
                if (err) throw err;
                if (user == null) socket.write("Please login first.\n% ");
                if (isNaN(recv[1]) || user["Mail"].length < recv[1] || recv[1] <= 0) {
                    socket.write("No such mail.\n% ");
                    return;
                }
                let delIdx = recv[1] - 1;
                let s3settings = {
                    Type: "delete-mail",
                    Bucket: user["Bucket"],
                    Key: user["Mail"][delIdx]["Key"]
                };
                socket.write(JSON.stringify(s3settings));
                // remove item from mail array
                user["Mail"].splice(delIdx, 1);
                // update new mail array for user
                User.updateOne({ Username: username }, { "$set": { "Mail": user["Mail"] } }, (err, success) => {
                    if (err) throw err;
                });
            });
        }
    }
};