var Router         = require('./router').Router,
    Window         = require('./window'),
    _App           = require('./bindings').App,
    NativeWindow   = require('./bindings').NativeWindow,
    WindowSettings = require('./settings').WindowSettings,
    AppSettings    = require('./settings').AppSettings;

var _init          = require('./bindings').init,
    decorate       = require('./utils').decorate,
    inherit        = require('./utils').inherit,
    isObject       = require('./utils').isObject,
    staticRouter   = require('./router').staticRouter;

var _createWindow  = _App.prototype.createWindow,
    concat         = Array.prototype.concat;

var pipedEvents    = ['create', 'minimize', 'maximize', 'fullscreen', 'restore', 'move', 'resize'];


module.exports = App;

function App(){
  var self = this;

  process.on('appjs-exit', function(){
    self.exit();
  });

  process.on('appjs-request', function(request, callback){
    self.emit('request', request, callback);
  });

  decorate(this, {
    screenHeight: _App.screenHeight,
    screenWidth : _App.screenWidth,
    initialized : false,
    windows     : {},
    router      : new Router(this),
  });
}

inherit(App, process.EventEmitter, [
  function createWindow(options){
    if (!this.initialized) {
      this.init();
    }
    var self = this;
    if (typeof options === 'string') {
      if (isObject(arguments[1])) {
        arguments[1].url = options;
        options = arguments[1];
      } else {
        options = { url: options };
      }
    }
    if (!(options instanceof WindowSettings)) {
      options = new WindowSettings(options);
    }

    var nativeWindow = _createWindow.call(this, options),
        window = new Window(nativeWindow);

    nativeWindow.name = options.name;
    this.windows[options.name] = window;

    nativeWindow.on('close', function(){
      window.emit('close');
      delete self.windows[options.name];
    });

    pipedEvents.forEach(function(event){
      nativeWindow.pipe(event, window);
    });

    this.emit('create-window', window);

    return window;
  },
  function exit(){
    this.emit('exit');

    Object.keys(this.windows).forEach(function(name){
      var window = this.windows[name];
      if (window && window.close) {
        window.close();
      }
    }, this);

    process.nextTick(function(){
      process.kill(process.pid);
    });
  },
  function serveFilesFrom(root){
    this.router.use(staticRouter(root));
    return this;
  },
  function init(options){
    if (this.initialized)
      throw new Error('AppJS already intiialized');

    options = new AppSettings(options);
    _init(options);
    this.intialized = true;
    this.emit('init', options);
    return this;
  }
]);



NativeWindow.prototype.__proto__ = process.EventEmitter.prototype;

decorate(NativeWindow.prototype, [
  function pipe(event, target){
    var emit = target.emit;
    this.on(event, function(){
      emit.apply(target, concat.apply(event, arguments));
    });
  }
]);
