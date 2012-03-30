/**
 * (The MIT License)
 *
 * Copyright (c) 2012 Morteza Milani <mrtz.milani@gmail.com> and other 
 * AppJS contributors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **/
 
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
    appname: 'appjs',
    title: 'Application',
    version: '0.0.1',
    compile: false,
    embed: false,
    entry_point: "./app.js",
    build_dir: './build',
    extra: [
    ]
}

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
    init : To initialize a new project. it creates a manifest file for you.\n\
    \n\
    package: It packages your file according to settings in manifest file.\n\
    \n\
    build: It builds your project according to settings in manifest file.\n\
";
