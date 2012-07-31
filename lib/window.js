var NativeWindow = require('./bindings').Window,
    App = require('./bindings').App,
    Ephemeral = require('./handlers').Ephemeral,
    ReferenceType = require('./handlers').ReferenceType,
    EventEmitter = process.EventEmitter,

    decorate = require('./utils').decorate,
    inherit = require('./utils').inherit,
    bridge = require('./bridge');


module.exports = Window;


function Window(nativeWindow){
  if (!(this instanceof Window))
    return new Window(nativeWindow);

  var events = this._events = {},
      window = new Ephemeral(this, { window: nativeWindow }),
      frame = this.frame = new Frame(nativeWindow),
      created = false;

  Object.defineProperty(this, '_events', { enumerable: false });

  function transition(){
    var windowProto = window.__proto__;
    windowProto._events = events;
    ['on', 'off', 'emit', 'once'].forEach(function(key){
      windowProto[key] = EventEmitter.prototype[key];
    });

    window.frame = frame;

    if (!created) {
      created = true;
      window.emit('create');
    }
    setTimeout(function(){
      window.emit('ready');
    }, 10);
  }

  nativeWindow.on('ready', function(){
    nativeWindow.runInBrowser(bridge);
    bridge(nativeWindow, window, Ephemeral.unwrap);
    setTimeout(transition, 10);
  });

  nativeWindow.on('context-released', function(){
    Ephemeral.unwrap(window).unmorph();
  });

  return window;
}

inherit(Window, EventEmitter, [
  function toString(){
    return '[object Window]';
  }
]);


var props = ['left', 'top', 'width', 'height', 'title', 'topmost', 'showChrome',
             'resizable', 'autoResize', 'opacity', 'alpha','state'];

var WindowReference = new ReferenceType(props, {
  get right(){
    return App.screenWidth() - this.left - this.width;
  },
  set right(v){
    if (this.resizable) {
      this.width = Math.max(0, App.screenWidth() - this.left - v);
    } else {
      this.left = App.screenWidth() - this.width - v;
    }
  },
  get bottom(){
    return App.screenHeight() - this.top - this.height;
  },
  set bottom(v){
    if (this.resizable) {
      this.height = Math.max(0, App.screenHeight() - this.top - v);
    } else {
      this.top = App.screenHeight() - this.height - v;
    }
  }
});

function Frame(win){
  if (!(win instanceof NativeWindow)) {
    throw new TypeError('Invalid Constructor Invocation');
  } else {
    return new WindowReference(win);
  }
}

WindowReference.prototype = Frame.prototype;
decorate(Frame.prototype, true, [
  function center(){
    ReferenceType.unwrap(this).move((App.screenWidth() - this.width) / 2, (App.screenHeight() - this.height) / 2);
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
  function move(left, top, width, height){
    ReferenceType.unwrap(this).move(left, top, width, height);
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
