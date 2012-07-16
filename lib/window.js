var NativeWindow = require('./bindings').Window,
    EventEmitter = process.EventEmitter,
    _hasOwn = Object.prototype.hasOwnProperty,
    handlers = new WeakMap;


module.exports = Window;

function Window(win){
  if (!(this instanceof Window))
    return new Window(win);

  var handler = new WindowHandler(win, this);
  var window = Proxy.create(handler, Window.prototype);
  handlers.set(window, handler);

  win.transition = function transition(newHandler, props){
    handler.__proto__ = newHandler;
    for (var k in props) {
      handler[k] = props[k];
    }
    return window;
  };

  win.imbue = function imbue(){
    var windowProto = window.__proto__;
    installMethods(windowProto);
    windowProto._events = handler.wrapper._events;
    ['on', 'off', 'emit', 'once'].forEach(function(key){
      windowProto[key] = EventEmitter.prototype[key];
    });
    window.emit('ready');
  };

  return window;
}

Object.keys(NativeWindow).forEach(function(key){
  if (typeof NativeWindow[key] === 'function') {
    Window[key] = NativeWindow[key];
  }
});

var installMethods = function(){
  var windowMethods = [
    'openDevTools', 'closeDevTools', 'minimize', 'maximize',
    'restore', 'drag', 'show', 'hide', 'setPosition', 'setSize'
  ].map(function(key){
    var method = NativeWindow.prototype[key];
    if (typeof method === 'function') {
      return new Function('h', 'm', 'return function '+key+'(){ m.apply(h.get(this).window, arguments); return this }')(handlers,method);
    }
  }).filter(Boolean);

  return function installMethods(o){
    windowMethods.forEach(function(method){
      o[method.name] = method;
    })
  }
}();

Window.prototype.__proto__ = EventEmitter.prototype;
installMethods(Window.prototype);



function WindowHandler(win, wrapper){
  this.window = win;
  this.wrapper = wrapper;
}

WindowHandler.prototype = {
  keys: function keys(){
    return Object.keys(this.wrapper);
  },
  enumerate: function enumerate(){
    var i=0, k=[];
    for (k[i++] in this.wrapper);
    return k;
  },
  getOwnPropertyNames: function getOwnPropertyNames(){
    return Object.getOwnPropertyNames(this.wrapper);
  },
  get: function get(rcvr, key){
    return this.wrapper[key];
  },
  set: function set(rcvr, key, value){
    this.wrapper[key] = value;
    return true;
  },
  has: function has(key){
    return key in this.wrapper;
  },
  hasOwn: function hasOwn(key){
    return _hasOwn.call(this.wrapper, key);
  },
  delete: function delete_(key){
    delete this.wrapper[key];
    return true;
  },
  defineProperty: function defineProperty(key, desc){
    Object.defineProperty(this.wrapper, key, desc);
    return true;
  },
  getOwnPropertyDescriptor: function getOwnPropertyDescriptor(key){
    var desc = Object.getOwnPropertyDescriptor(this.wrapper, key);
    if (desc) {
      desc.configurable = true;
    }
    return desc;
  }
};
