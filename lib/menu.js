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

  // OSX does not support mnemonics for menu,
  // it only supports submenu mnemonics 
  if( process.platform == 'darwin') {
    for(i in options) {
      options[i]['label'] = options[i]['label'].replace(/(&)(\w)/g,'$2');
    }
  }

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
      options[i]['icon'] = path.resolve(__dirname,'../../../..',options[i]['icon']);
    }
    normalize(options[i]['submenu']);
  }
}
