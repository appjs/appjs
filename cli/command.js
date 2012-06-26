var fs = require('fs');
var path = require('path');
var util = require('./util');
var pack = require('./pack');
var mkdir = require('mkdirp');
var config = require('./config');
var exec = require('child_process').exec;

var init = exports.init = function(){

}

var packageF = exports.package = function(){

}

var help = exports.help = function(){
  console.log(config.HELP);
}