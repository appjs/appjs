var path = require('path'),
    router = require('./lib/router'),
    bindings = require('./lib/bindings'),
    browserInit = require('./lib/browser-init'),
    bridge = require('./lib/bridge');

var App = bindings.App,
    Window = bindings.Window,
    _createWindow = App.prototype.createWindow,
    _send = Window.prototype.send,
    _extend = require('./lib/utils').extend;


// TODO: windows specific component is temporary
if (process.platform === 'win32') {
  var stylesForWindow = require('./lib/windowStyles')(Window);
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
    var window = _createWindow.call(this, url, settings);
    Object.defineProperties(window, {
      id: { value: this.windows.length },
      _events: { value: {} }
    });
    this.windows.push(window);

    window.once('create', function(){
      stylesForWindow(window);
      window.runInBrowser(browserInit);
    });

    window.once('ready', function(){
      bridge(window);
      window.runInBrowser(bridge);
    });

    window.on('close', function(){
      var id = this.id;
      process.nextTick(function(){
        self.windows[id] = null;
      });
    });

    return window;
  },
  send: function send(id, type, msg){
    if (arguments.length === 2) {
      msg = type;
      type = id;
      id = 0;
    }
    if (id in this.windows) {
      return this.windows[id].send(type, msg);
    }
  }
});

var IPC = {
  encode: function encode(msg){
    if (msg == null)
      return '{}';
    else if (typeof msg === 'object')
      return JSON.stringify(msg);
    else if (typeof msg === 'string')
      return msg;
    else
      throw new TypeError("Tried to encode invalid type");
  },
  decode: function decode(msg){
    try {
      return JSON.parse(msg);
    } catch (e) {
      return { error: e };
    }
  }
};


Window.prototype.__proto__ = process.EventEmitter.prototype;

_extend(Window.prototype, {
  onmessage: function onmessage(msg){
    var res = {};
    msg = IPC.decode(msg);

    if (msg && msg.type && this._events[msg.type]) {
      this.emit(msg.type, msg.msg, res);
    }

    return IPC.encode(res.result === undefined ? {} : res.result);
  },
  send: function send(type, msg){
    msg = { type: type, msg: msg };
    return IPC.decode(_send.call(this, IPC.encode(msg)));
  }
});


var locales_dir = path.resolve(__dirname,'data','pak');

var app = bindings.init.call(null,locales_dir);

app.windows = [];

app.extend(router);

app.on("exit",function(){
  process.nextTick(function(){
    process.exit();
  });
});

module.exports = app;
