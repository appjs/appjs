if (typeof Proxy !== 'object' || typeof WeakMap !== 'function') {
  throw new Error ('AppJS requires Node is run with the --harmony command line flag');
}



var Router         = require('./router').Router,
    Window         = require('./window'),
    _App           = require('./bindings').App,
    NativeWindow   = require('./bindings').NativeWindow,
    WindowSettings = require('./settings').WindowSettings,
    AppSettings    = require('./settings').AppSettings;

var _init         = require('./bindings').init,
    decorate      = require('./utils').decorate,
    isObject      = require('./utils').isObject,
    staticRouter  = require('./router').staticRouter;


var concat = Array.prototype.concat,
    _createWindow = _App.prototype.createWindow;

var events = ['create', 'minimize', 'maximize', 'fullscreen', 'restore', 'move', 'resize'];



NativeWindow.prototype.__proto__ = process.EventEmitter.prototype;

decorate(NativeWindow.prototype, [
  function pipe(event, target){
    var emit = target.emit;
    this.on(event, function(){
      emit.apply(target, concat.apply(event, arguments));
    });
  }
]);

function App(){
  var self = this;

  decorate(this, {
    screenWidth: _App.screenWidth,
    screenHeight: _App.screenHeight,
    windows: {},
    initialized: false,
    router: new Router(this),
    init: function init(options){
      if (this.initialized)
        throw new Error('AppJS already intiialized');

      _init(new AppSettings(options));
      this.intialized = true;
      return this;
    }
  });

  this.on('exit',function(){
    self.exit();
    process.nextTick(function(){
      process.kill(process.pid);
    });
  });
};

decorate(App.prototype, [
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

    events.forEach(function(event){
      nativeWindow.pipe(event, window);
    });

    return window;
  },
  function exit(){
    Object.keys(this.windows).forEach(function(name){
      var window = this.windows[name];
      if (window && window.close) {
        window.close();
      }
    }, this);
  },
  function serveFilesFrom(root){
    this.router.use(staticRouter(root));
    return this;
  },
  function on(type, listener){
    process.on('appjs-'+type, listener.bind(this));
  }
]);

module.exports = new App;
