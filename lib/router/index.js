var Response   = require('./response'),
    Request    = require('./request'),
    Class      = require('../utils').Class;

var methods = ['get', 'post'];

var Router = module.exports = new Class({
  constructor: function Router(emitter){
    var self = this;

    this.handlers = { all: [] };
    for (var i=0; i < methods.length; i++) {
      this.handlers[methods[i]] = [];
    }

    emitter.on('request',function(request, callback){
      self.handle(new Request(request), new Response(callback));
    });
  },
  handle: function handle(request, response){
    var listeners = this.handlers[request.method] || [],
        index = -1;
    void function next(){
      if (++index === listeners.length) {
        response.send(500, 'Internal Server Error');
      } else {
        listeners[index](request, response, next);
      }
    }();
  },
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
      if (request.method.toLowerCase() === method && request.pathname === route) {
        callback(request, response, next);
      } else {
        next();
      }
    });
    return this;
  }
});

