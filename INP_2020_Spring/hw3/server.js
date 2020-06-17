const net = require("net");
// from hw1
const userfunc = require("./userfunc.js");
// hw2
const board = require("./board.js");
// hw3
const mail = require("./mail.js");

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
        });

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
                    } break;
                case "login":
                    if (recv.length !== 3) {
                        socket.write("Usage: login <username> <password>\n% ");
                    } else {
                        socket["Session"] = userfunc.login(socket, recv);
                    } break;
                case "logout":
                    if (recv.length !== 1) {
                        socket.write("Usage: logout\n% ");
                    } else {
                        userfunc.logout(socket);
                    } break;
                case "whoami":
                    if (recv.length !== 1) {
                        socket.write("Usage: whoami\n% ");
                    } else {
                        userfunc.whoami(socket);
                    } break;
                // === hw 2 ===
                case "create-board":
                    if (recv.length !== 2) {
                        socket.write("Usage: create-board <name>\n% ");
                    } else {
                        board.create_board(socket, recv);
                    } break;
                case "list-board":
                    if (recv.length > 2) {
                        socket.write("Usage: list-board ##<key>\n% ");
                    } else {
                        board.list_board(socket, recv);
                    } break;
                case "create-post":
                    if (recv.length < 6) {
                        socket.write("Usage: create-post <board-name> --title <title> --content <content>\n% ");
                    } else {
                        board.create_post(socket, recv, data);
                    } break;
                case "list-post":
                    if (recv.length < 2 || recv.length > 3) {
                        socket.write("Usage: list-post <board-name> ##<key>\n% ");
                    } else {
                        board.list_post(socket, recv);
                    } break;
                case "read":
                    if (recv.length !== 2) {
                        socket.write("Usage: read <post-id>\n% ");
                    } else {
                        board.read(socket, recv);
                    } break;
                case "comment":
                    if (recv.length < 3) {
                        socket.write("Usage: comment <post-id> <comment>\n% ");
                    } else {
                        board.comment(socket, recv, data);
                    } break;
                case "delete-post":
                    if (recv.length !== 2) {
                        socket.write("Usage: delete-post <post-id>\n% ");
                    } else {
                        board.delete_post(socket, recv);
                    } break;
                case "update-post":
                    if (recv.length < 4) {
                        socket.write("Usage: update-post <post-id> --title/content <new>\n% ");
                    } else {
                        board.update_post(socket, recv, data);
                    } break;
                // === hw3 ===
                case "mail-to":
                    if (recv.length < 6) {
                        socket.write("Usage: mail-to <username> --subject <subject> --content <content>\n% ");
                    } else {
                        mail.mail_to(socket, recv, data);
                    } break;
                case "list-mail":
                    if (recv.length !== 1) {
                        socket.write("Usage: list-mail\n% ");
                    } else {
                        mail.list_mail(socket);
                    } break;
                case "retr-mail":
                    if (recv.length !== 2) {
                        socket.write("Usage: retr-mail <mail#>\n% ");
                    } else {
                        mail.retr_mail(socket, recv);
                    } break;
                case "delete-mail":
                    if (recv.length !== 2) {
                        socket.write("Usage: delete-mail <mail#>\n% ");
                    } else {
                        mail.delete_mail(socket, recv);
                    } break;
                default:
                    socket.write("% "); break;
            }
        } else {
            // empty input
            socket.write("% ");
        }
    });
})

if (process.argv.length !== 3) {
    server.listen(9999);
}
else {
    server.listen(process.argv[2]);
}