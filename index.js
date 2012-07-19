var path = require('path'),
    router = require('./lib/router'),
    bindings = require('./lib/bindings'),
    Window = require('./lib/window');

var App = bindings.App,
    NativeWindow = bindings.Window,
    _createWindow = App.prototype.createWindow,
    _extend = require('./lib/utils').extend;


NativeWindow.prototype.__proto__ = process.EventEmitter.prototype;

// TODO: windows specific component is temporary
if (process.platform === 'win32') {
  var stylesForWindow = require('./lib/windowStyles')(NativeWindow);
} else {
  var stylesForWindow = function(){}
}

_extend(App.prototype, {
  extend: function extend(mod){
    _extend(this, mod.prototype);
    mod.call(this);
  },
  createWindow: function createWindow(url, settings){
    if (url !== null && typeof url === 'object') {
      settings = url;
      url = settings.url;
    }
    url = url || 'http://appjs/';
    settings = _extend({}, settings);
    if (settings.icons) {
      ['smaller', 'small', 'big', 'bigger'].forEach(function(size){
        settings.icons[size] = path.resolve(settings.icons[size] || '');
      });
    } else {
      settings.icons = {};
    }

    var self = this;
    var id = this.windows.length;
    var nativeWindow = _createWindow.call(this, url, settings);
    nativeWindow.id = id;
    nativeWindow._events = {};
    var window = new Window(nativeWindow);
    window._events = {};
    Object.defineProperty(window, '_events', { enumerable: false });
    this.windows.push(window);

    nativeWindow.on('create', function(){
      window.emit('create');
    });

    nativeWindow.on('close', function(){
      window.emit('close');
    });

    return window;
  }
});


var app = bindings.init.call(null, path.resolve(__dirname,'data','pak'));
app.windows = [];
app.extend(router);
app.on("exit",function(){
  process.nextTick(function(){ process.exit() });
});

module.exports = app;
