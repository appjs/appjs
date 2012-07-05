var Response     = require('./response'),
    Request      = require('./request'),
    staticRouter = require('./static-router'),
    Class        = require('../utils').Class;

var methods = ['get', 'post'];

var Router = module.exports = new Class({
  constructor: function Router(){
    var self = this;
    this.handlers = {};
    for (var i=0; i < methods.length; i++) {
      this.handlers[methods[i]] = [];
    }

    this.on('request',function(request, callback){
      request.method = request.method.toLowerCase();
      var listeners = self.handlers[request.method];

      if (listeners && listeners.length) {
        var response = new Response(callback);
        var index = -1;
        request = new Request(request);
        void function next(){
          if (++index < listeners.length) {
            listeners[index](request, response, next);
          } else {
            response.send(500, 'Internal Server Error');
          }
        }();
      }
    });
    return this;
  },
  staticRouter: staticRouter,
  use: function use(middleware){
    for (var i=0; i < methods.length; i++) {
      this.handlers[methods[i]].push(middleware);
    }
    return this;
  },
  all: function(route, callback) {
    for (var i=0; i < methods.length; i++) {
      this[methods[i]](route, callback);
    }
    return this;
  }
});

methods.forEach(function(method){
  Router.prototype[method] = function(route, callback) {
    this.handlers[method].push(function(request, response, next){
      if (request.method === method && request.pathname === route) {
        callback(request, response, next);
      } else {
        next();
      }
    });
    return this;
  }
});
