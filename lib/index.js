var path = require('path'),
    router = require('./router'),
    bindings = require('./bindings'),
    Window = require('./window');

var App = bindings.App,
    NativeWindow = bindings.Window,
    _createWindow = App.prototype.createWindow,
    _extend = require('./utils').extend;


NativeWindow.prototype.__proto__ = process.EventEmitter.prototype;
NativeWindow.prototype.pipe = function pipe(event, target){
  this.on(event, function(a, b, c, d){
    switch (arguments.length) {
      case 0: return target.emit(event);
      case 1: return target.emit(event, a);
      case 2: return target.emit(event, a, b);
      case 3: return target.emit(event, a, b, c);
      case 4: return target.emit(event, a, b, c, d);
      default: target.emit.apply(target, [].concat.apply(event, arguments));
    }
  });
}

// TODO: windows specific component is temporary
if (process.platform === 'win32') {
  var stylesForWindow = require('./windowStyles')(NativeWindow);
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

    nativeWindow.pipe('create', window);
    nativeWindow.pipe('close', window);
    nativeWindow.pipe('minimized', window);
    nativeWindow.pipe('maximized', window);
    nativeWindow.pipe('restored', window);
    nativeWindow.pipe('move', window);
    nativeWindow.pipe('resize', window);
    return window;
  }
});


var app = bindings.init.call(null, bindings.locals);
app.windows = [];
app.extend(router);
app.on("exit",function(){
  process.nextTick(function(){ process.exit() });
});

module.exports = app;
