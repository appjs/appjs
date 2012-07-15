var NativeWindow = require('./bindings').Window,
    EE = process.EventEmitter.prototype;
var _call      = Function.prototype.call,
    _apply     = Function.prototype.apply,
    _bind      = Function.prototype.bind,
    _slice     = Array.prototype.slice,
    _hasOwn    = Object.prototype.hasOwnProperty;

var handlers = new WeakMap,
    windows = new WeakMap

function defineHidden(o, k, v){
  return Object.defineProperty(o, k, {
    configurable: true,
    writable: true,
    enumerable: false,
    value: v
  });
}

module.exports = Window;

function Window(win){
  if (windows.has(win))
    return windows.get(win);

  if (!(this instanceof Window))
    return new Window(win);

  win.transition = function transition(newHandler, props){
    handler.__proto__ = newHandler;
    for (var k in props) {
      handler[k] = props[k];
    }
    return proxy;
  };

  win.imbue = function imbue(){
    var proto = proxy.__proto__;
    imbueWindow(proto);
    proto._events = handler.wrapper._events;
    ['on', 'off', 'emit', 'once'].forEach(function(key){
      proto[key] = EE[key];
    });
    proxy.emit('ready');
  };

  var handler = new WindowHandler(win, this);
  var proxy = Proxy.create(handler, Window.prototype);
  handlers.set(proxy, handler);
  return proxy;
}

Object.keys(NativeWindow).forEach(function(key){
  if (typeof NativeWindow[key] === 'function') {
    Window[key] = NativeWindow[key];
  }
});


var windowFuncs = ['openDevTools', 'closeDevTools', 'minimize', 'maximize', 'restore',
                   'drag', 'show', 'hide', 'setPosition', 'setSize']

function imbueWindow(o){
  windowFuncs.forEach(function(key){
    if (typeof NativeWindow.prototype[key] === 'function') {
      o[key] = function(){
        return NativeWindow.prototype[key].apply(handlers.get(this).window, arguments);
      };
    }
  });
}

Window.prototype.__proto__ = EE;
imbueWindow(Window.prototype);

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
