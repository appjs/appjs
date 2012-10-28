var fs   = require('fs'),
    path = require('path');

var NativeWindow    = require('./bindings').NativeWindow,
    App             = require('./bindings').App,
    Ephemeral       = require('./handlers').Ephemeral,
    ReferenceType   = require('./handlers').ReferenceType,
    EventEmitter    = require('events').EventEmitter,
    browserRequire  = require('./browser-require');

var decorate = require('./utils').decorate,
    inherit  = require('./utils').inherit,
    bridge   = require('./bridge'),
    log      = require('./utils').log;

var _ = ReferenceType.unwrap;

function toFinite(n){
  return isFinite(n /= 1) ? n : 0;
}

function int(n, max){
  if (n < 1 && n >= -1) {
    n *= max;
  }
  return isFinite(n /= 1) ? n : 0;
}

function round(n, d){
  return +n.toFixed(d || 2);
}

module.exports = Window;

function Window(nativeWindow){
  if (!(this instanceof Window))
    return new Window(nativeWindow);

  var events = this._events = {},
      window = new Ephemeral(this, { window: nativeWindow }),
      frame = this.frame = new Frame(nativeWindow);

  Object.defineProperty(this, '_events', { enumerable: false });

  nativeWindow.on('ready', function(){
    nativeWindow.runInBrowser(bridge);
    bridge(nativeWindow, window, Ephemeral.unwrap);

    setTimeout(function(){
      var windowProto = window.__proto__;
      windowProto._events = events;
      ['on', 'emit', 'once'].forEach(function(key){
        windowProto[key] = EventEmitter.prototype[key];
      });
      windowProto.off = EventEmitter.prototype.removeListener;

      window.frame = frame;
      browserRequire(window);

      window.emit('ready');
      window.dispatchEvent(new window.Event('app-ready'));
    }, 1);
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


var scr = Object.defineProperties({}, {
  w: { get: App.screenWidth  },
  h: { get: App.screenHeight },
});

var props = ['title', 'topmost', 'showChrome', 'showOnTaskbar', 'showOnPager', 'resizable', 'autoResize', 'opacity', 'alpha', 'state'];

var WindowReference = new ReferenceType(props, {
  get    left( ){ return this.left   },
  get     top( ){ return this.top    },
  get   width( ){ return this.width  },
  get  height( ){ return this.height },
  get   right( ){ return scr.w - this.left - this.width  },
  get  bottom( ){ return scr.h - this.top  - this.height },
  get centerX( ){ return this.left + this.width / 2  },
  get centerY( ){ return this.top + this.height / 2 },
  set    left(v){ v = int(v, scr.w); this.move(v, this.top, this.width + this.left - v, this.height); },
  set     top(v){ v = int(v, scr.h); this.move(this.left, v, this.width, this.height + this.top - v); },
  set   width(v){ this.width  = int(v, scr.w) },
  set  height(v){ this.height = int(v, scr.h) },
  set   right(v){ this.width  = scr.w - this.left - int(v, scr.w) },
  set  bottom(v){ this.height = scr.h - this.top  - int(v, scr.h) },
  set centerX(v){ this.left = int(v, scr.w) - this.width  / 2 },
  set centerY(v){ this.top  = int(v, scr.h) - this.height / 2 },
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
    _(this).move((scr.w - this.width) / 2, (scr.h - this.height) / 2);
    return this;
  },
  function setMenuBar(options){ _(this).setMenuBar(options); return this; },
  function openDialog(settings,cb){
    // enum NW_DIALOGTYPE
    var types = [
      "save",
      "open",
      "font",
      "color"
    ];

    settings.type = types.indexOf(settings.type);

    settings.acceptTypes || (settings.acceptTypes = {});

    if (Array.isArray(settings.acceptTypes)) {
      var types = settings.acceptTypes;
      settings.acceptTypes = {};
      settings.acceptTypes[types.join('|')] = types;
    }

    settings.acceptTypes = Object.keys(settings.acceptTypes).reduce(function(acc, key) {
      var val = settings.acceptTypes[key]
      acc.push(key + ':' + val.join(';'));
      return acc;
    }, []).join(',');


    _(this).openDialog(settings,cb);
    return this;
  },
  function openDevTools(){ _(this).openDevTools(); return this; },
  function closeDevTools(){ _(this).closeDevTools(); return this; },
  function drag(){ _(this).drag(); return this; },
  function minimize(){ _(this).minimize(); return this; },
  function maximize(){ _(this).maximize(); return this; },
  function restore(){ _(this).restore(); return this; },
  function destroy(){ _(this).destroy(); return this; },
  function fullscreen(){ _(this).fullscreen(); return this; },
  function show(){ _(this).show(); return this; },
  function focus(){ _(this).focus(); return this; },
  function hide(){ _(this).hide(); return this; },
  function resize(width, height){ _(this).resize(width, height); return this; },
  function move(left, top, width, height){
    _(this).move(left, top, width, height);
    return this;
  },
  function setIcon(size, pathname){
    pathname = path.resolve(pathname);
    if (fs.existsSync(pathname)) {
      _(this).setIcon(size, pathname);
    }
    return this;
  },
  function toString(){ return '[object Frame]'; },
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

