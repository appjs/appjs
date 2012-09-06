var Response   = require('./Response'),
    Request    = require('./Request'),
    decorate   = require('../utils').decorate;

var methods = ['get', 'post'];


module.exports = Router;

function Router(emitter){
  var self = this;

  this.handlers = { all: [] };
  for (var i=0; i < methods.length; i++) {
    this.handlers[methods[i]] = [];
  }

  emitter.on('request',function(request, callback){
    request = new Request(request);
    response = new Response(request, callback);
    self.handle(request, response);
  });
}

decorate(Router.prototype, [
  function handle(request, response){
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
  function use(middleware){
    for (var i=0; i < methods.length; i++) {
      this.handlers[methods[i]].push(middleware);
    }
    return this;
  },
  function all(route, callback) {
    for (var i=0; i < methods.length; i++) {
      this[methods[i]](route, callback);
    }
    return this;
  }
]);

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

