// global variables
let subscription = { SubbedTopics: [] };
let loggedin = false;

// for tcp connection
const net = require("net");
let connectHost = undefined;
let listenPort = undefined;
if (process.argv.length !== 4) {
    connectHost = "127.0.0.1";
    listenPort = 9999;
} else {
    connectHost = process.argv[2];
    listenPort = process.argv[3];
}
const client = net.createConnection({ port: listenPort, host: connectHost });
let stdin = process.openStdin();
stdin.addListener("data", (input) => {
    let send = String(input).replace(/\r|\n|\t/g, " ").split(" ").filter((value, index, arr) => { if (value != "") return value });
    if (send.length > 0 && send[0] === "exit") {
        process.exit();
    } else if (send[0] === "subscribe") {
        if (loggedin) {
            let usageArr = ["--board", "--author"];
            if (send.length !== 5 || !(usageArr.indexOf(send[1]) > -1) || send[3] !== "--keyword") {
                process.stdout.write("Usage: subscribe --board/--author <board-name>/<author-name> --keyword <keyword>\n% ");
            } else {
                let type = send[1] === "--board" ? "Board" : "Author";
                let topic = send[2];
                let keyword = send[4];
                if (subscription[topic] == null) {
                    // create new array to save keywords
                    subscription[topic] = [{
                        Type: type,
                        Keyword: keyword,
                        Timestamp: Date.now()
                    }];
                    addTopic(topic);
                    subscription["SubbedTopics"].push(topic);
                } else {
                    // check if keyword had subscribed already
                    for (let sub of subscription[topic]) {
                        if (sub["Keyword"] === keyword) {
                            process.stdout.write("Already subscribed\n% ");
                            return;
                        }
                    }
                    subscription[topic].push({
                        Type: type,
                        Keyword: keyword,
                        Timestamp: Date.now()
                    });
                }
                process.stdout.write("Subscribe successfully\n% ");
            }
        } else {
            process.stdout.write("Please login first\n% ");
        }
    } else if (send[0] === "unsubscribe") {
        let usageArr = ["--board", "--author"];
        if (send.length !== 3 || !(usageArr.indexOf(send[1]) > -1)) {
            process.stdout.write("Usage: unsubscribe --board/--author <board-name>/<author-name>\n% ");
        } else {
            let unsubTopic = send[2];
            if (loggedin) {
                if (subscription[unsubTopic] == null) {
                    process.stdout.write("You haven't subscribed " + unsubTopic + "\n% ");
                } else {
                    consumer.removeTopics([unsubTopic], (err, rmv) => { if (err) throw err; });
                    subscription["SubbedTopics"].splice(subscription["SubbedTopics"].indexOf(unsubTopic), 1);
                    delete subscription[unsubTopic];
                    process.stdout.write("Unsubscribe successfully\n% ");
                }
            } else {
                process.stdout.write("Please login first\n% ");
            }
        }
    } else if (send[0] === "list-sub") {
        if (loggedin && send.length === 1) {
            let authors = { SubTopics: [] };
            let boards = { SubTopics: [] };
            for (let sub of subscription["SubbedTopics"]) {
                if (subscription[sub][0]["Type"] === "Board") {
                    boards["SubTopics"].push(sub);
                    boards[sub] = [];
                    for (let keywords of subscription[sub]) {
                        boards[sub].push(keywords["Keyword"]);
                    }
                } else {
                    authors["SubTopics"].push(sub);
                    authors[sub] = [];
                    for (let keywords of subscription[sub]) {
                        authors[sub].push(keywords["Keyword"]);
                    }
                }
            }
            if (authors.SubTopics.length > 0) {
                process.stdout.write("Author: ");
                for (let sub of authors.SubTopics) {
                    process.stdout.write(sub + ": ");
                    for (let i = 0; i < authors[sub].length; i++) {
                        process.stdout.write(authors[sub][i]);
                        if (i !== authors[sub].length - 1) {
                            process.stdout.write(", ");
                        } else {
                            process.stdout.write("; ");
                        }
                    }
                }
            }
            if (boards.SubTopics.length > 0) {
                if (authors.SubTopics.length > 0) process.stdout.write("\n");
                process.stdout.write("Board: ");
                for (let sub of boards.SubTopics) {
                    process.stdout.write(sub + ": ");
                    for (let i = 0; i < boards[sub].length; i++) {
                        process.stdout.write(boards[sub][i]);
                        if (i !== boards[sub].length - 1) {
                            process.stdout.write(", ");
                        } else {
                            process.stdout.write("; ");
                        }
                    }
                }
            }
            if (subscription["SubbedTopics"].length > 0) {
                process.stdout.write("\n% ");
            } else {
                process.stdout.write("% ");
            }
        } else if (send.length !== 1) {
            process.stdout.write("Usage: list-sub\n% ");
        } else {
            process.stdout.write("Please login first\n% ");
        }
    } else {
        client.write(input.toString());
    }
});

function isJSON(str) {
    if (/^\s*$/.test(str)) return false;
    str = str.replace(/\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g, '@');
    str = str.replace(/"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g, ']');
    str = str.replace(/(?:^|:|,)(?:\s*\[)+/g, '');
    return (/^[\],:{}\s]*$/).test(str);
}

client.on("data", (data) => {
    if (!isJSON(String(data))) {
        process.stdout.write(data.toString());
    } else {
        let recv = JSON.parse(String(data));
        switch (recv["Type"]) {
            case "StatusUpdate": {
                loggedin = recv["Status"];
                let name = recv["Name"];
                switch (recv["LogType"]) {
                    case "login":
                        process.stdout.write("Welcome, " + name + ".\n% ");
                        break;
                    case "logout":
                        process.stdout.write("Bye, " + name + ".\n% ");
                        if (subscription["SubbedTopics"].length > 0) {
                            consumer.removeTopics(subscription["SubbedTopics"], (err, rmv) => { if (err) throw err; });
                        }
                        subscription = { SubbedTopics: [] };
                        break;
                }
            }
                break;
            case "AskProduce": {
                let title = recv["Title"];
                let author = recv["Author"];
                let board = recv["Board"];
                let msg = "*[" + board + "] " + title + " - by " + author + "* ";
                let payload = [{
                    topic: board,
                    key: Date.now(),
                    messages: msg
                }, {
                    topic: author,
                    key: Date.now(),
                    messages: msg
                }];
                if (producer.ready) {
                    producer.send(payload, (err, data) => {
                        if (err) throw err;
                    });
                }
                process.stdout.write("Create post successfully.\n% ");
            } break;
            default:
                break;
        }
    }
});

// for kafka
const kafka = require("kafka-node");
let kClient = new kafka.KafkaClient();

// producer
let Producer = kafka.HighLevelProducer;
let producer = new Producer(kClient);
producer.on("error", (err) => { if (err) throw err; });

// consumer
let Consumer = kafka.Consumer;
let consumer = new Consumer(kClient, [], { autoCommit: false });
consumer.on("message", (message) => {
    let recv = message;
    let topic = recv["topic"];
    let msg = recv["value"];
    let timestamp = new Date(Number(recv["key"]));
    if (subscription[topic] != null) {
        for (let sub of subscription[topic]) {
            if (sub["Timestamp"] <= timestamp) {
                let splitMsg = msg.split(" ");
                let prefix = splitMsg[0].length + 1;
                let suffix = (" " + splitMsg[splitMsg.length - 3] + " " + splitMsg[splitMsg.length - 2] + " " + splitMsg[splitMsg.length - 1]).length;
                let title = msg.substring(prefix, msg.length - suffix);
                if (title.includes(sub["Keyword"])) {
                    process.stdout.write(msg);
                }
            }
        }
    }
});

function addTopic(newTopic) {
    consumer.addTopics([newTopic], (err, added) => {
        if (err) { // topic does not exists
            kClient.createTopics([{ topic: newTopic, partitions: 1, replicationFactor: 1 }], (err, res) => {
                if (err) throw err;
                // add created topic to consumer
                consumer.addTopics([newTopic], (err, added) => {
                    if (err) throw err;
                });
            });
        }
    });
}
