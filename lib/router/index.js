var methods      = require('./methods')
  , Response     = require('./response')
  , Request      = require('./request')
  , staticRouter = require('./static_router');

var req_handlers = [];

function Next(req, res) {

  var index = -1;

  var method = req.method.toLowerCase();

  var next = function() {
    if( ++index < req_handlers[method].length ) {
      req_handlers[method][index].apply(null,[req,res,next]);
    } else {
      res.send(500,'Internal Server Error');
    }   

  }

  return next;
}

module.exports = function router() {

  var self = this;

  methods.forEach(function(method){

    req_handlers[method] = [];

    self[method] = function(route,callback) {
      
      var handler = function(req,res,next){

        if( req.method.toLowerCase() == method ) {
          //TODO match route with pathname.
          if( req.pathname == route ) {
            callback.call(null,req,res,next);
            return;
          }

        }

        next();

      };

      req_handlers[method].push(handler);
    }

    self['all'] = function(route,callback) {
      methods.forEach(function(method){
        self[method].call(null,route,callback);
      })
    }

  });

  this.staticRouter = staticRouter;

  this.use = function(middleware) {
    methods.forEach(function(method){
      req_handlers[method].push(middleware);
    })
  }

  this.on("request",function(req,callback){

    req  = new Request(req);
    res  = new Response(callback);
    next = new Next(req,res);

    next();
  });

}