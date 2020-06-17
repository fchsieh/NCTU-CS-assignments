const mongoose = require("mongoose");
const Schema = mongoose.Schema;

let User = new Schema({
    Username: String,
    Password: String,
    Email: String
});

mongoose.model("User", User);
mongoose.connect("mongodb://localhost/userdata", { useNewUrlParser: true, useUnifiedTopology: true });
mongoose.connection.on("error", console.error.bind(console, "MongoDB connection failed:"));
