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

exports.ERROR = {
    tooMuchArguments: 'Too much arguments, needs one.'
};

exports.HELP = "\n\
Welcome to AppJS!\n\
\n\
\n\
Usage: appjs <command>\n\
\n\
where <command> is one of:\n\
\n\
    init : To initialize a new project. it creates a boilerplate appjs app for you.\n\
    \n\
    package: It creates bundles for you project.\n\
";
