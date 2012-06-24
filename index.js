var path = require('path');
var appjs = require('./build/Release/appjs.node');
var router = require('./lib/router');

var Init = appjs.init;

appjs.init = function() {

  var app = Init.apply(null,arguments);

  app.extend = function(mod) {
    mod.call(this);
  }

  app.router = new router(app);
//  app.extend(router);

  app.on("window_close",function(){
    process.nextTick(function(){
      process.exit();
    });
  });

  var createWindow = app.createWindow;

  app.createWindow = function(url,settings){
    if( settings.icons ) {
      settings.icons['smaller'] = path.resolve(settings.icons['smaller']);
      settings.icons['small'] = path.resolve(settings.icons['small']);
      settings.icons['big'] = path.resolve(settings.icons['big']);
      settings.icons['bigger'] = path.resolve(settings.icons['bigger']);
    }

    return createWindow.call(app,url,settings);
  }

  return app;
}

module.exports = appjs;
