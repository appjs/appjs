if (typeof Proxy !== 'object' || typeof WeakMap !== 'function') {
  throw new Error ('AppJS requires Node is run with the --harmony command line flag');
}



var Router         = require('./router').Router,
    Window         = require('./window'),
    App            = require('./bindings').App,
    NativeWindow   = require('./bindings').NativeWindow,
    WindowSettings = require('./settings').WindowSettings,
    AppSettings    = require('./settings').AppSettings;

var init          = require('./bindings').init,
    decorate      = require('./utils').decorate,
    isObject      = require('./utils').isObject,
    staticRouter  = require('./router').staticRouter;


var concat = Array.prototype.concat,
    _createWindow = App.prototype.createWindow;

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

decorate(App.prototype, [
  function createWindow(options){
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


var app = init(new AppSettings);

decorate(app, {
  screenWidth: App.screenWidth,
  screenHeight: App.screenHeight,
  windows: {},
  router: new Router(app)
});

app.on('exit',function(){
  app.exit();
  process.nextTick(function(){
    process.kill(process.pid);
  });
});

module.exports = app;
