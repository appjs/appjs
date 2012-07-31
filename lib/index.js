var bindings      = require('./bindings'),
    settings      = require('./settings'),
    decorate      = require('./utils').decorate,
    isObject      = require('./utils').isObject,
    staticRouter  = require('./router/static-router');

var Router        = require('./router'),
    Window        = require('./window'),
    App           = require('./bindings').App,
    NativeWindow  = require('./bindings').NativeWindow;

var concat = Array.prototype.concat,
    _createWindow = App.prototype.createWindow;

var events = ['minimize', 'maximize', 'fullscreen', 'restore', 'move', 'resize'];



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
    if (!(options instanceof settings.WindowSettings)) {
      options = new settings.WindowSettings(options);
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
  }
]);



var app = module.exports = bindings.init(new settings.AppSettings(bindings.initConfig));

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
