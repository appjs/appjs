var NativeWindow = require('./bindings').Window,
    App = require('./bindings').App,
    Ephemeral = require('./handlers').Ephemeral,
    ReferenceType = require('./handlers').ReferenceType,
    EventEmitter = process.EventEmitter,

    screenHeight = App.prototype.screenHeight,
    screenWidth = App.prototype.screenWidth,

    decorate = require('./utils').decorate,
    bridge = require('./bridge');


module.exports = Window;


function Window(nativeWindow){
  if (!(this instanceof Window))
    return new Window(nativeWindow);

  var self = this,
      window = new Ephemeral(this, { window: nativeWindow }),
      frame = this.frame = new Frame(nativeWindow);

  nativeWindow.on('ready', function(){
    nativeWindow.runInBrowser(bridge);
    bridge(nativeWindow, window, Ephemeral.unwrap);
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
    Ephemeral.unwrap(window).unmorph();
  });

  return window;
}

Window.prototype = decorate(Object.create(EventEmitter.prototype), true, {
  constructor: Window,
  toString: function toString(){
    return '[object Window]';
  }
});



var props = ['left', 'top', 'width', 'height', 'title', 'topmost', 'showChrome', 'resizable', 'opacity', 'alpha','state'];

var FrameRef = new ReferenceType(props, {
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
  if (!(win instanceof NativeWindow)) {
    throw new TypeError('Invalid Constructor Invocation');
  } else {
    return new FrameRef(win);
  }
}

FrameRef.prototype = Frame.prototype;
decorate(Frame.prototype, true, [
  function center(){
    ReferenceType.unwrap(this).move((screenHeight() - this.height) / 2, (screenWidth() - this.width) / 2);
    return this;
  },
  function drag(){
    ReferenceType.unwrap(this).drag();
    return this;
  },
  function minimize(){
    ReferenceType.unwrap(this).minimize();
    return this;
  },
  function maximize(){
    ReferenceType.unwrap(this).maximize();
    return this;
  },
  function restore(){
    ReferenceType.unwrap(this).restore();
    return this;
  },
  function fullscreen(){
    ReferenceType.unwrap(this).fullscreen();
    return this;
  },
  function show(){
    ReferenceType.unwrap(this).show();
    return this;
  },
  function hide(){
    ReferenceType.unwrap(this).hide();
    return this;
  },
  function move(top, left, width, height){
    ReferenceType.unwrap(this).move(top, left, width, height);
    return this;
  },
  function resize(width, height){
    ReferenceType.unwrap(this).resize(width, height)
    return this
  },
  function openDevTools(){
    ReferenceType.unwrap(this).openDevTools();
  },
  function closeDevTools(){
    ReferenceType.unwrap(this).closeDevTools();
  },
  function toString(){
    return '[object Frame]';
  },
  function fade(to, duration, callback){
    var self = this;
    var from = this.opacity;
    callback = arguments[arguments.length - 1];
    duration = duration > 10 ? duration : to > 1 ? to : 300;
    if (!(to >= 0 && to <= 1)) {
      to = from > .5 ? 0 : 1;
    }
    if (from === to) return;

    duration /= 15;
    var step = (to - from) / duration;
    var clamp = clamper(from > to, to);

    void function fade(){
      self.opacity = clamp(self.opacity + step);
      if (self.opacity <= 0 || self.opacity >= 1) {
        typeof callback == 'function' && callback.call(self, self.opacity);
      } else {
        setTimeout(fade, 15);
      }
    }();
  }
]);

function clamper(isMin, val){
  return isMin
    ? function(n){ return n < val ? val : n }
    : function(n){ return n > val ? val : n };
}
