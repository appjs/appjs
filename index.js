var appjs = require('./build/Release/appjs.node');
var router = require('./lib/router');

var Init = appjs.init;

appjs.init = function() {

  var app = Init.apply(null,arguments);

  app.extend = function(mod) {
    mod.call(this);
  }

  app.extend(router);

  return app;
}

module.exports = appjs;