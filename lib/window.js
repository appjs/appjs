var NativeWindow = require('./bindings').Window,
    App = require('./bindings').App,
    EventEmitter = process.EventEmitter,

    screenHeight = App.prototype.screenHeight,
    screenWidth = App.prototype.screenWidth,

    decorate = require('./utils').decorate,
    bridge = require('./bridge'),

    _hasOwn = Object.prototype.hasOwnProperty,
    _slice = Array.prototype.slice,
    _apply = Function.prototype.apply,
    _bind = Function.prototype.bind,

    frames = new WeakMap,
    handlers = new WeakMap;


function unwrap(o){
  return frames.get(o);
}

module.exports = Window;


function Window(nativeWindow){
  if (!(this instanceof Window))
    return new Window(nativeWindow);

  var self = this,
      handler = new WindowHandler(nativeWindow, this),
      window = Proxy.create(handler, Window.prototype),
      frame = this.frame = new Frame(nativeWindow);

  nativeWindow.on('ready', function(){
    nativeWindow.runInBrowser(bridge);
    bridge(nativeWindow, handler, window);
    setTimeout(function(){
      var windowProto = window.__proto__;
      windowProto._events = self._events;
      ['on', 'off', 'emit', 'once'].forEach(function(key){
        windowProto[key] = EventEmitter.prototype[key];
      });
      window.frame = frame;
      window.emit('ready');
    }, 100);
  });

  nativeWindow.on('context-released', function(){
    handler.__proto__ = WindowHandler.prototype;
  });

  return window;
}

Window.prototype = decorate(Object.create(EventEmitter.prototype), true, {
  constructor: Window,
  toString: function toString(){
    return '[object Window]';
  }
});



function Reflector(target){
  this.target = target;
}

decorate(Reflector.prototype, [
  function keys(){
    return Object.keys(this.target);
  },
  function enumerate(){
    var i=0, k=[];
    for (k[i++] in this.target);
    return k;
  },
  function getOwnPropertyNames(){
    return Object.getOwnPropertyNames(this.target);
  },
  function get(rcvr, key){
    return this.target[key];
  },
  function set(rcvr, key, value){
    this.target[key] = value;
    return true;
  },
  function has(key){
    return key in this.target;
  },
  function hasOwn(key){
    return _hasOwn.call(this.target, key);
  },
  function delet\u0065(key){
    delete this.target[key];
    return true;
  },
  function defineProperty(key, desc){
    Object.defineProperty(this.target, key, desc);
    return true;
  },
  function getOwnPropertyDescriptor(key){
    var desc = Object.getOwnPropertyDescriptor(this.target, key);
    desc && (desc.configurable = true);
    return desc;
  },
  function apply(rcvr, args){
    return _apply.call(this.target, rcvr, args);
  },
  function construct(args){
    return new (_bind.apply(this.target, [null].concat(args)));
  }
]);



function WindowHandler(nativeWindow, target){
  this.window = nativeWindow;
  this.target = target;
}

WindowHandler.prototype = Object.create(Reflector.prototype);

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
    var handler = new RefHandler(ref, this);
    var proxy = Proxy.create(handler, Creator.prototype);
    handlers.set(proxy, handler);
    return proxy;
  };
}


decorate(RefType.prototype, [
  function keys(){
    return this.names.concat(Object.keys(this.target));
  },
  function enumerate(){
    var i = this.names.length, k = this.names.slice();
    for (k[i++] in this.target);
    return k;
  },
  function getOwnPropertyNames(){
    return this.names.concat(Object.getOwnPropertyNames(this.target));
  },
   function get(rcvr, key){
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
  function set(rcvr, key, value){
    if (key in this.refs) {
      this.ref[this.refs[key]] = value;
    } else if (key in this.accessors) {
      this.accessors[key].set.call(this.ref, value);
    } else {
      this.target[key] = value;
    }
    return true;
  },
  function has(key){
    return key in this.refs || key in this.accessors || key in this.target;
  },
  function hasOwn(key){
    return key in this.refs || key in this.accessors || _hasOwn.call(this.target, key);
  },
  function delet\u0065(key){
    if (key in this.refs) {
      delete this.ref[this.refs[key]];
    } else {
      delete this.target[key];
    }
    return true;
  },
  function defineProperty(key, desc){
    if (key in this.refs) {
      Object.defineProperty(this.ref, this.refs[key], desc);
    } else if (key in this.accessors) {
      this.accessors[key].set.call(this.ref, desc.value);
    } else {
      Object.defineProperty(this.target, key, desc);
    }
    return true;
  },
  function getOwnPropertyDescriptor(key){
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
  function apply(rcvr, args){
    return _apply.call(this.ref, rcvr, args);
  },
  function construct(args){
    return new (_bind.apply(this.ref, [null].concat(args)));
  }
]);

var props = ['left', 'top', 'width', 'height', 'title', 'topmost', 'showChrome', 'resizable', 'opacity', 'alpha']//,'state',];

var FrameImpl = new RefType(props, {
  get right(){
    return screenWidth() - this.left - this.width;
  },
  set right(v){
    if (this.resizable) {
      this.width = Math.max(0, screenWidth() - this.left - v);
    } else {
      this.left = screenWidth() - this.width - v;
    }
  },
  get bottom(){
    return screenHeight() - this.top - this.height;
  },
  set bottom(v){
    if (this.resizable) {
      this.height = Math.max(0, screenHeight() - this.top - v);
    } else {
      this.top = screenHeight() - this.height - v;
    }
  }
});

function Frame(win){
  if (!(win instanceof NativeWindow))
    throw new TypeError('Invalid Constructor Invocation');

  var frame = new FrameImpl(win);
  frames.set(frame, win);
  return frame;
}

FrameImpl.prototype = Frame.prototype;
decorate(Frame.prototype, true, [
  function center(){
    unwrap(this).move((screenHeight() - this.height) / 2, (screenWidth() - this.width) / 2);
    return this;
  },
  function drag(){
    unwrap(this).drag();
    return this;
  },
  function minimize(){
    unwrap(this).minimize();
    return this;
  },
  function maximize(){
    unwrap(this).maximize();
    return this;
  },
  function restore(){
    unwrap(this).restore();
    return this;
  },
  function fullscreen(){
    unwrap(this).fullscreen();
    return this;
  },
  function show(){
    unwrap(this).show();
    return this;
  },
  function hide(){
    unwrap(this).hide();
    return this;
  },
  function move(top, left, width, height){
    unwrap(this).move(top, left, width, height);
    return this;
  },
  function resize(width, height){
    unwrap(this).resize(width, height)
    return this
  },
  function openDevTools(){
    unwrap(this).openDevTools();
  },
  function closeDevTools(){
    unwrap(this).closeDevTools();
  },
  function toString(){
    return '[object Frame]';
  },
  // function fade(to, duration, callback){
  //   var self = this;
  //   var from = this.opacity;
  //   callback = arguments[arguments.length - 1];
  //   duration = duration > 10 ? duration : to > 1 ? to : 300;
  //   if (!(to >= 0 && to <= 1)) {
  //     to = from > .5 ? 0 : 1;
  //   }
  //   if (from === to) return;

  //   duration /= 15;
  //   var step = (to - from) / duration;
  //   var clamp = clamper(from > to, to);

  //   void function fade(){
  //     self.opacity = clamp(self.opacity + step);
  //     if (self.opacity <= 0 || self.opacity >= 1) {
  //       typeof callback == 'function' && callback.call(self, self.opacity);
  //     } else {
  //       setTimeout(fade, 15);
  //     }
  //   }();
  // }
]);

function clamper(isMin, val){
  return isMin
    ? function(n){ return n < val ? val : n }
    : function(n){ return n > val ? val : n };
}
