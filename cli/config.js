var path = require('path');
var util = require('./util');

var logEnum = [];

Object.keys(util.log.LEVELS).forEach(function (level) {
  logEnum.push(level);
});

exports.arg_types = {
    "compile": Boolean,
    "embed":   Boolean,
    "entry":   String,
    "out":     String,
    "extra":   Array,
    "log"   :  logEnum
};

exports.arg_shorts = {
    c: ["--compile"],
    e: ["--embed"],
    t: ["--entry"],
    p: ["--extra"],
    o: ["--out"],
    l: ["--log"]
};

exports.manifest = {
    compile: false,
    embed: false,
    entry_point: "app.js",
    build_dir: './build',
    extra: [
    ]
}

exports.ERROR = {
    tooMuchArguments: 'Too much arguments, needs one.'
};