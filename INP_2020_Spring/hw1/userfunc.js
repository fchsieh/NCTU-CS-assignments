require("./db.js");
const mongoose = require("mongoose");
// Schemas from db.js
const User = mongoose.model("User");

module.exports = {
    register: function (socket, recv) {
        let name = recv[1];
        let email = recv[2];
        let password = recv[3];

        // Check if username already exists
        User.countDocuments({ Username: name }, (err, count) => {
            if (err) {
                throw err;
            }
            // user exists
            if (count > 0) {
                socket.write("Username is already used.\n% ");
                return;
            } else {
                new User({
                    Username: name,
                    Password: password,
                    Email: email
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

        // check if user exists
        User.countDocuments({ Username: name }, (err, count) => {
            if (err) {
                throw err;
            }
            if (count > 0) {
                // check password
                User.findOne({ Username: name }, (err, user) => {
                    if (err) {
                        throw err;
                    }
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
                });
            } else {
                // not found
                socket.write("Login failed.\n% ");
                return session;
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