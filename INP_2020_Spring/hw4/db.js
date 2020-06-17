const mongoose = require("mongoose");
const Schema = mongoose.Schema;
const AutoIncrement = require('mongoose-sequence')(mongoose);

const UserSchema = new Schema({
    Username: String,
    Password: String,
    Email: String
});

const BoardSchema = new Schema({
    BoardName: String,
    Mod: String,
});
BoardSchema.plugin(AutoIncrement, { inc_field: "BoardIndex" });

const PostSchema = new Schema({
    Board: String,
    Author: String,
    Title: String,
    Content: String,
    CreatedAt: Schema.Types.Mixed,
    Comments: [{
        Commentator: String,
        Comment: String,
    }]
});
PostSchema.plugin(AutoIncrement, { inc_field: "PostIndex" });

// connect to db
mongoose.connect("mongodb://localhost/nphw", { useNewUrlParser: true, useUnifiedTopology: true, useCreateIndex: true, useFindAndModify: false });
const db = mongoose.connection;
db.on("error", console.error.bind(console, "Connect to mongoDB failed:"));

// set Schema
const User = mongoose.model("Users", UserSchema);
const Board = mongoose.model("Boards", BoardSchema);
const Post = mongoose.model("Posts", PostSchema);

module.exports = { User, Board, Post };