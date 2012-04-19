var appjs = require('./build/Release/appjs.node');
var EventEmitter = require('events').EventEmitter;
var Extend = require('util')._extend;

var Init = appjs.init;

appjs.init = function(){

  var obj = Init(arguments);

  
  return obj;

}

module.exports = appjs;
