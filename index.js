var path = require('path');
var appjs = require('./build/Release/appjs.node');
var router = require('./lib/router');

var Init = appjs.init;

appjs.init = function() {

  var app = Init.apply(null,arguments);

  app.extend = function(mod) {
    mod.call(this);
  }

  app.extend(router);

  var createWindow = app.createWindow;

  app.createWindow = function(url,settings){

    settings.icons['smaller'] = path.resolve(settings.icons['smaller']);
    settings.icons['small'] = path.resolve(settings.icons['small']);
    settings.icons['big'] = path.resolve(settings.icons['big']);
    settings.icons['bigger'] = path.resolve(settings.icons['bigger']);

    return createWindow(url,settings);
  }

  return app;
}

module.exports = appjs;