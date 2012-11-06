var fs   = require('fs'),
    path = require('path');

var NativeMenu    = require('./bindings').NativeMenu,
    EventEmitter  = require('events').EventEmitter,
    inherit  = require('./utils').inherit;

module.exports = Menu;

function Menu(options){

  normalizeItems(options);

  var nativeMenu = new NativeMenu(options);

  inherit(nativeMenu, EventEmitter, [
    function toString(){
      return '[object Menu]';
    }
  ]);

  return nativeMenu;
}

var normalizeItems = function(options){
  normalizeMnemonic(options);
  normalize(options);
}

var normalizeMnemonic = function(options){

  if( process.platform != 'linux') {
    return;
  }

  for(i in options) {
    options[i]['label'] = options[i]['label'].replace(/(&)(\w)/g,'_$2');
    normalizeMnemonic(options[i]['submenu']);
  }
}

var normalize = function(options){
  for(i in options) {
    if(options[i].hasOwnProperty('icon')){
      options[i]['icon'] = path.resolve(options[i]['icon']);
    }
    normalize(options[i]['submenu']);
  }
}
