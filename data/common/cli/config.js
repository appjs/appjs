var path = require('path');
var util = require('./util');

var logEnum = [];

Object.keys(util.log.LEVELS).forEach(function (level) {
  logEnum.push(level);
});

exports.arg_types = {
    "version": String,
};

exports.arg_shorts = {
    v: ["--version"],
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
    init: Create a sample folder structure and a launcher.\n\
    \n\
    run: Run the application.\n\
    \n\
    package: Package the app, ready to distribute.\n\
";
