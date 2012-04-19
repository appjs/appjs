var appjs = require('./build/Release/appjs.node');
var EventEmitter = require('events').EventEmitter;

var Init = appjs.init;

appjs.init = function(){

  var obj = Init.apply(this,arguments);

  for (var k in EventEmitter.prototype)
    obj.__proto__[k] = EventEmitter.prototype[k];

  return obj;

}

module.exports = appjs;
