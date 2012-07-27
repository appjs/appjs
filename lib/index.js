var bindings = require('./bindings'),
    router   = require('./router'),
    decorate = require('./utils').decorate,
    isObject = require('./utils').isObject;

var App           = bindings.App,
    NativeWindow  = bindings.Window,
    Window        = require('./window'),
    Icons         = require('./utils').Icons;

var concat = Array.prototype.concat,
    _createWindow = App.prototype.createWindow;

var events = ['close', 'minimize', 'maximize', 'fullscreen',
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
    if (url && typeof url === 'object') {
      settings = url;
      url = settings.url;
    }
    settings = decorate({}, settings);
    settings.icons = new Icons(settings.icons);

    var nativeWindow = _createWindow.call(this, url || 'http://appjs/', settings),
        window = new Window(nativeWindow);

    events.forEach(function(event){
      nativeWindow.pipe(event, window);
    });

    return window;
  },
]);



var app = module.exports = bindings.init(bindings.initConfig);
app.screenWidth = App.screenWidth;
app.screenHeight = App.screenHeight;
app.windows = [];
app.extend(router);
app.on('exit',function(){
  process.nextTick(function(){
    process.exit();
  });
});
