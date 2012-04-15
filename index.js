//module.exports = require('./build/Release/appjs.node');

var appjs = require('./build/Release/appjs.node');

/*var init = appjs.init;

appjs.init = function(){

  var obj = init(arguments);
  
  var window = obj.createWindow();
  
//  obj.__proto__ = Object.create(window,obj.__proto__);
  
  return obj;

}*/

module.exports = appjs;
