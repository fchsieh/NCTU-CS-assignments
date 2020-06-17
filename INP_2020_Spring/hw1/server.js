const net = require("net");
const userfunc = require("./userfunc.js");

let UserSession = {
    "name": null,
    "login": false,
}

const server = net.createServer((socket) => {
    socket["Session"] = new Object();
    socket["Session"].name = null;
    socket["Session"].login = false;
});

server.on("connection", (socket) => {
    console.log("New connection.");
    // Print welcome message
    socket.write("********************************\n** Welcome to the BBS server. **\n********************************\n");
    socket.write("% ");

    socket.on("data", (data) => {
        let recv = String(data).replace(/\r|\n|\t/g, " ").split(" ").filter((value, index, arr) => {
            if (value != "")
                return value
        })

        if (recv.length > 0) {
            switch (recv[0]) {
                case "exit":
                    socket.destroy();
                    return;
                case "register":
                    if (recv.length !== 4) {
                        socket.write("Usage: register <username> <email> <password>\n% ");
                    } else {
                        userfunc.register(socket, recv);
                    }
                    break
                case "login":
                    if (recv.length !== 3) {
                        socket.write("Usage: login <username> <password>\n% ");
                    } else {
                        socket["Session"] = userfunc.login(socket, recv);
                    }
                    break
                case "logout":
                    if (recv.length !== 1) {
                        socket.write("Usage: logout\n% ");
                    } else {
                        userfunc.logout(socket);
                    }
                    break
                case "whoami":
                    if (recv.length !== 1) {
                        socket.write("Usage: whoami\n% ");
                    } else {
                        userfunc.whoami(socket);
                    }
                    break
                default:
                    socket.write("% ");
                    break
            }
        } else {
            socket.write("% ");
        }
    });
})

server.listen(process.argv[2]);