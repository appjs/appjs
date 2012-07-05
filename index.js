var path = require('path'),
    router = require('./lib/router'),
    bindings = module.exports = require('./lib/bindings');


var init = bindings.init;

bindings.init = function() {
  var app = init.apply(null, arguments);

  app.extend = function extend(mod){
    Object.keys(mod.prototype).forEach(function(key){
      app[key] = mod.prototype[key];
    });
    mod.call(app);
  }

  app.extend(router);

  app.on("window_close", function(){
    process.nextTick(function(){
      process.exit();
    });
  });

  var createWindow = app.createWindow;

  app.createWindow = function(url,settings){

    if (settings.icons) {
      ['smaller', 'small', 'big', 'bigger'].forEach(function(size){
        settings.icons[size] = path.resolve(settings.icons[size]);
      });
    }

    return createWindow.call(app,url,settings);
  }

  return app;
}

bindings;
