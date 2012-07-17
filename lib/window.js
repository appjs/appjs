var NativeWindow = require('./bindings').Window,
    App = require('./bindings').App,
    EventEmitter = process.EventEmitter,
    _hasOwn = Object.prototype.hasOwnProperty,
    _slice = Array.prototype.slice,
    _apply = Function.prototype.apply,
    _bind = Function.prototype.bind,
    handlers = new WeakMap;


module.exports = Window;

function Window(win){
  if (!(this instanceof Window))
    return new Window(win);

  var self = this;
  var handler = new WindowHandler(win, this);
  var window = Proxy.create(handler, Window.prototype);
  handlers.set(window, handler);

  this.frame = new Frame(win);

  win.transition = function transition(newHandler, props){
    handler.__proto__ = newHandler;
    for (var k in props) {
      handler[k] = props[k];
    }
    return window;
  };

  win.untransition = function untransition(){
    handler.__proto__ = WindowHandler.prototype;
    delete self.frame;
  };

  win.imbue = function imbue(){
    var windowProto = window.__proto__;
    installMethods(windowProto);
    windowProto._events = handler.target._events;
    ['on', 'off', 'emit', 'once'].forEach(function(key){
      windowProto[key] = EventEmitter.prototype[key];
    });
    window.console.open = function open(){ win.openDevTools() };
    window.console.close = function close(){ win.closeDevTools() };
    window.frame = self.frame;
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
    'restore', 'drag', 'show', 'hide', 'move', 'resize'
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


function Reflector(target){
  this.target = target;
}

Reflector.prototype = {
  keys: function keys(){
    return Object.keys(this.target);
  },
  enumerate: function enumerate(){
    var i=0, k=[];
    for (k[i++] in this.target);
    return k;
  },
  getOwnPropertyNames: function getOwnPropertyNames(){
    return Object.getOwnPropertyNames(this.target);
  },
  get: function get(rcvr, key){
    return this.target[key];
  },
  set: function set(rcvr, key, value){
    this.target[key] = value;
    return true;
  },
  has: function has(key){
    return key in this.target;
  },
  hasOwn: function hasOwn(key){
    return _hasOwn.call(this.target, key);
  },
  delete: function delete_(key){
    delete this.target[key];
    return true;
  },
  defineProperty: function defineProperty(key, desc){
    Object.defineProperty(this.target, key, desc);
    return true;
  },
  getOwnPropertyDescriptor: function getOwnPropertyDescriptor(key){
    var desc = Object.getOwnPropertyDescriptor(this.target, key);
    desc && (desc.configurable = true);
    return desc;
  },
  apply: function apply(rcvr, args){
    return _apply.call(this.target, rcvr, args);
  },
  construct: function construct(args){
    return new (_bind.apply(this.target, [null].concat(args)));
  }
};



function RefType(properties, accessors){
  this.refs = Object.create(null);
  if (Array.isArray(properties)) {
    this.names = properties;
    properties.forEach(function(key){
      this[key] = key;
    }, this.refs);
  } else {
    this.names = Object.keys(properties);
    this.names.forEach(function(key){
      this[key] = properties[key];
    }, this.refs);
  }

  this.accessors = Object.create(null);
  if (accessors) {
    var accessorNames = Object.keys(accessors);
    this.names = this.names.concat(accessorNames);
    accessorNames.forEach(function(key){
      this[key] = Object.getOwnPropertyDescriptor(accessors, key);
    }, this.accessors);
  }

  function RefHandler(ref, target){
    this.ref = ref;
    this.target = target;
  }

  RefHandler.prototype = this;

  return function Creator(ref){
    return Proxy.create(new RefHandler(ref, this), Creator.prototype);
  };
}

RefType.prototype = {
  keys: function keys(){
    return this.names.concat(Object.keys(this.target));
  },
  enumerate: function enumerate(){
    var i = this.names.length, k = this.names.slice();
    for (k[i++] in this.target);
    return k;
  },
  getOwnPropertyNames: function getOwnPropertyNames(){
    return this.names.concat(Object.getOwnPropertyNames(this.target));
  },
  get: function get(rcvr, key){
    if (key === '__proto__') {
      return this.target.__proto__;
    } else if (key in this.refs) {
      return this.ref[this.refs[key]];
    } else if (key in this.accessors) {
      return this.accessors[key].get.call(this.ref);
    } else {
      return this.target[key];
    }
  },
  set: function set(rcvr, key, value){
    if (key in this.refs) {
      this.ref[this.refs[key]] = value;
    } else if (key in this.accessors) {
      this.accessors[key].set.call(this.ref, value);
    } else {
      this.target[key] = value;
    }
    return true;
  },
  has: function has(key){
    return key in this.refs || key in this.accessors || key in this.target;
  },
  hasOwn: function hasOwn(key){
    return key in this.refs || key in this.accessors || _hasOwn.call(this.target, key);
  },
  delete: function delete_(key){
    if (key in this.refs) {
      delete this.ref[this.refs[key]];
    } else {
      delete this.target[key];
    }
    return true;
  },
  defineProperty: function defineProperty(key, desc){
    if (key in this.refs) {
      Object.defineProperty(this.ref, this.refs[key], desc);
    } else if (key in this.accessors) {
      this.accessors[key].set.call(this.ref, desc.value);
    } else {
      Object.defineProperty(this.target, key, desc);
    }
    return true;
  },
  getOwnPropertyDescriptor: function getOwnPropertyDescriptor(key){
    if (key in this.refs) {
      var desc = Object.getOwnPropertyDescriptor(this.ref, this.refs[key]);
    } else if (key in this.accessors) {
      return {
        enumerable: true, configurable: true, writable: true,
        value: this.accessors[key].get.call(this.ref)
      };
    } else {
      var desc = Object.getOwnPropertyDescriptor(this.target, key);
    }
    desc && (desc.configurable = true);
    return desc;
  },
  apply: function apply(rcvr, args){
    return _apply.call(this.ref, rcvr, args);
  },
  construct: function construct(args){
    return new (_bind.apply(this.ref, [null].concat(args)));
  }
};

var screen = {
  height: App.prototype.screenHeight,
  width: App.prototype.screenWidth
};

var FrameImpl = new RefType(['left', 'top', 'width', 'height', 'title', 'state'], {
  get right(){
    return screen.width() - this.left - this.width;
  },
  set right(v){
    this.width = Math.max(0, screen.width() - this.left - v);
  },
  get bottom(){
    return screen.height() - this.top - this.height;
  },
  set bottom(v){
    this.height = Math.max(0, screen.height() - this.top - v);
  }
});

function Frame(win){
  var frame = new FrameImpl(win);
  frames.set(frame, win);
  return frame;
}

var frames = new WeakMap;

Frame.prototype = FrameImpl.prototype = {
  constructor: Frame,
  center: function center(){
    frames.get(this).move((screen.height() - this.height) / 2, (screen.width() - this.width) / 2);
    return this;
  },
  minimize: function minimize(){
    frames.get(this).minimize();
    return this;
  },
  maximize: function maximize(){
    frames.get(this).maximize();
    return this;
  },
  restore: function restore(){
    frames.get(this).restore();
    return this;
  },
  fullscreen: function fullscreen(){
    frames.get(this).fullscreen();
    return this;
  },
  show: function show(){
    frames.get(this).show();
    return this;
  },
  hide: function hide(){
    frames.get(this).hide();
    return this;
  },
  move: function move(top, left, width, height){
    frames.get(this).move(top, left, width, height);
    return this;
  },
  resize: function resize(width, height){
    frames.get(this).resize(width, height)
    return this
  }
};

function WindowHandler(win, target){
  this.window = win;
  this.target = target;
}

WindowHandler.prototype = Object.create(Reflector.prototype);
