const User = require("./db.js").User;

module.exports = {
    register: function (socket, recv) {
        let name = recv[1];
        let email = recv[2];
        let password = recv[3];

        // Check if username had been used
        User.countDocuments({ Username: name }, (err, count) => {
            if (err) throw err;
            // user exists
            if (count > 0) {
                socket.write("Username is already used.\n% ");
                return;
            } else {
                // Create bucket: send the generated bucket name to the client
                let bucket = "0513328-" + name.toLowerCase() + "-" + Date.now();
                let s3settings = {
                    Type: "bucket-name",
                    Name: bucket
                };
                socket.write(JSON.stringify(s3settings));
                // Save to local db
                new User({
                    Username: name,
                    Password: password,
                    Email: email,
                    Bucket: bucket
                }).save((err) => {
                    if (err) {
                        // should not be in here.
                        console.error("Add user failed; check mongodb");
                        return;
                    } else {
                        socket.write("Register successfully.\n% ");
                        return;
                    }
                });
            }
        });
        return;
    },

    login: function (socket, recv) {
        let name = recv[1];
        let password = recv[2];
        let session = socket["Session"];

        if (socket["Session"].login) {
            // User already logged in
            socket.write("Please logout first.\n% ");
            return session;
        }

        User.findOne({ Username: name }, (err, user) => {
            if (err) throw err;
            if (user == null) {
                // user not found
                socket.write("Login failed.\n% ");
                return session;
            } else {
                if (user.Password === password) {
                    // login succeeded
                    socket.write("Welcome, " + name + ".\n% ");
                    session.name = name;
                    session.login = true;
                    return session;
                } else {
                    // wrong password!
                    socket.write("Login failed.\n% ");
                    return session;
                }
            }
        });
        return session;
    },

    logout: function (socket) {
        let session = socket.Session;
        let name = socket.Session.name;
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
        } else {
            session["name"] = null;
            session["login"] = false;
            socket.write("Bye, " + name + ".\n% ");
        }
        return session;
    },

    whoami: function (socket) {
        let name = socket.Session.name;
        if (!socket.Session.login) {
            socket.write("Please login first.\n% ");
        } else {
            socket.write(name + ".\n% ");
        }
    }
};