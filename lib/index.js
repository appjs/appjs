var bindings = require('./bindings'),
    config = require('./settings')(bindings.initConfig),
    router   = require('./router'),
    decorate = require('./utils').decorate,
    isObject = require('./utils').isObject;

var App           = bindings.App,
    NativeWindow  = bindings.Window,
    Window        = require('./window'),
    Icons         = require('./utils').Icons;

var concat = Array.prototype.concat,
    _createWindow = App.prototype.createWindow;

var names = require('./utils').names,
    events = ['minimize', 'maximize', 'fullscreen',
              'restore', 'move', 'resize', 'title-change'];



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
  function extend(mod){
    Object.keys(mod.prototype).forEach(function(key){
      Object.defineProperty(this, key, {
        configurable: true,
        writable: true,
        value: mod.prototype[key]
      });
    }, this);
    mod.call(this);
  },
  function createWindow(url, settings){
    var self = this;
    if (url && typeof url === 'object') {
      settings = url;
      url = settings.url;
    }
    settings = decorate({}, settings);
    settings.icons = new Icons(settings.icons);
    if (!settings.name) {
      settings.name = names[names.index++] + names.suffox;
      if (names.index === names.length) {
        names.index = 0;
        names.suffox = (names.suffox || 0) + 1;
      }
    }

    var nativeWindow = _createWindow.call(this, url || 'http://appjs/', settings),
        window = new Window(nativeWindow);

    nativeWindow.name = settings.name;
    this.windows[settings.name] = window;

    nativeWindow.on('close', function(){
      window.emit('close');
      delete self.windows[settings.name];
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
  }
]);



var app = module.exports = bindings.init(config);
app.screenWidth = App.screenWidth;
app.screenHeight = App.screenHeight;
app.windows = {};
app.extend(router);
app.on('exit',function(){
  app.exit();
  process.nextTick(function(){
    process.kill(process.pid);
  });
});
