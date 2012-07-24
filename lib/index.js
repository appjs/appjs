var path = require('path'),
    fs = require('fs'),
    router = require('./router'),
    bindings = require('./bindings'),
    Icons = require('./utils').Icons,
    Window = require('./window');

var App = bindings.App,
    NativeWindow = bindings.Window,
    _createWindow = App.prototype.createWindow,
    decorate = require('./utils').decorate,
    isObject = require('./utils').isObject,
    concat = [].concat;

NativeWindow.prototype.__proto__ = process.EventEmitter.prototype;
NativeWindow.prototype.pipe = function pipe(event, target){
  this.on(event, function(){
    target.emit.apply(target, concat.apply([event], arguments));
  });
}

var logPath = path.resolve(module.parent && path.dirname(module.parent.filename) || __dirname);
if (fs.existsSync(logPath + '/appjs.log'))
  logPath = path.resolve(logPath, 'appjs.log');
else if (fs.existsSync(logPath + '/content'))
  logPath = path.resolve(logPath, 'content', 'appjs.log');
else
  logPath = path.resolve(logPath, 'appjs.log');


decorate(App.prototype, [
  function extend(mod){
    Object.keys(mod.prototype).forEach(function(key){
      Object.defineProperty(this, key, {
        configurable: true,
        writable: true,
        value: mod.prototype[key]
      });
    }, this);
    mod.call(this);
  },
  function createWindow(url, settings){
    if (url !== null && typeof url === 'object') {
      settings = url;
      url = settings.url;
    }
    url = url || 'http://appjs/';
    settings = decorate({}, settings);
    settings.icons = new Icons(settings.icons);

    var self = this;
    var id = this.windows.length;
    var nativeWindow = _createWindow.call(this, url, settings);
    nativeWindow.id = id;
    nativeWindow._events = {};
    var window = new Window(nativeWindow);
    window._events = {};
    Object.defineProperty(window, '_events', { enumerable: false });
    this.windows.push(window);

    ['create', 'close', 'minimize', 'maximize', 'fullscreen', 'restore', 'move', 'resize', 'title-change'].forEach(function(event){
      nativeWindow.pipe(event, window);
    });
    return window;
  },
  function log(type, context, msg){
    var err;
    if (isError(type)) err = type, type = null;
    else if (isError(context)) err = context, context = null;
    else if (isError(msg)) err = msg, msg = null;
    if (err) {
      if (!msg) msg = err.stack || err.message;
      if (!type) type = err.message || err.name;
    }
    var out = '';
    if (type) out = type + ': ';
    if (msg) out += msg;
    if (context && typeof context === 'object') {
      try { context = JSON.stringify(context) }
      catch (e) { context += '' }
      out += '[ ' + context + ' ]';
    }
    if (out) {
      var file = this.logPath || logPath;
      try { fs.writeFileSync(logPath, out) }
      catch (e) { console.log(e, out) }
    }
  }
]);

function isError(o){
  if (!o || typeof o !== 'object') return false;
  if (Error.prototype.isPrototypeOf(o)) return true;
  if (/Error$/.test(o.name)) return true;
  if ('stack' in o) return true;
  return false;
}


var app = bindings.init(bindings.initConfig);
app.windows = [];
app.extend(router);
app.on("exit",function(){
  process.nextTick(function(){ process.exit() });
});

module.exports = app;
