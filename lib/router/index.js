var methods      = require('./methods')
  , Response     = require('./response')
  , Request      = require('./request')
  , Route        = require('./route')
  , staticRouter = require('./static_router')
  , utils        = require('../utils');

exports = module.exports = Router;

var req_handlers = [];
var global_handlers = [];

/**
 * The hierarchy of handlers:
 * 1^ Global Handlers
 * 2^ Method Handlers
 * 3^ Route Handler
 *
 * This to add params or functionality to req/res before user's callback
 * 
 * @param {Object} options
 * @api private
 */

function Next(app, Router, req, res) {

  var index = -1;
  var index_global = -1;

  var method = req.method.toLowerCase();

  var next = function() {

    if (++index_global < global_handlers.length)
      global_handlers[index_global].call(app, req, res, next);

    else if (req_handlers[method] && ++index < req_handlers[method].length)
      req_handlers[method][index].apply(null,[req,res,next]);

    else
      Router.dispatch(req, res);

  }

  return next;
}

/**
 * Initialize a new `Router` with the given `options`.
 * 
 * @param {Object} options
 * @api private
 */


function Router(app, options) {
  options = options || {};
  var self = this;
  this.map = {};
  this.params = {};
  this.caseSensitive = options.caseSensitive;
  this.strict = options.strict;
  this.app = app;
  app.router = this;


/**
 * This creates methods app.get(), app.post(), app.all(), etc.
 *
 * @param {Function} middleware
 * @param {String} method
 * @return {void}
 */

  methods.forEach(function(method) {
    app[method] = function(route, callback) {
      self.route(method, route, callback);
    }
  });

  app.all = function(route, callback) {
    methods.forEach(function(method){
      self.route(method, route, callback);
    })
  }


/**
 * Apply `middleware` on each request | on each method request
 *
 * @param {Function} middleware
 * @param {String} method
 * @return {void}
 */

  app.use = function(middleware, method) {
    if(method == undefined)
      global_handlers.push(middleware);
    else
      (req_handlers[method.toLowerCase()] = req_handlers[method.toLowerCase()] || []).push(middleware);
  }


  app.on("request",function(req,res){
    req  = new Request(req);
    res  = new Response(res);
    new Next(app, self, req, res) ();
  });

  app.staticRouter = staticRouter;
}


/**
 * Apply the right route callback
 *
 * @param {Function} middleware
 * @param {String} method
 * @return {void}
 * @api private
 */

Router.prototype.dispatch = function(req, res) {
  var method = req.method.toLowerCase();
  if(!this.map[method]) throw new Error('Error 1');
  this.map[method].forEach(function(route) {
    if(route.match(req.pathname)) {
      req.params = route.params;
      route.callback(req,res);
    }
  });

  //For debug: if no method has sent data
  if(!res.sent)
    res.send(500, '500 - No data sent for ['+req.pathname+']');
}


/**
 * Route `method`, `path` and one callback.
 *
 * @param {String} method
 * @param {String} path
 * @param {Function} callback
 * @return {Router} for chaining
 * @api private
 */

Router.prototype.route = function(method, path, callback){
  var method = method.toLowerCase()

  // ensure path was given
  if (!path) throw new Error('Router#' + method + '() requires a path');

  // create the route
  var route = new Route(method, path, callback, {
      sensitive: this.caseSensitive
    , strict: this.strict
  });

  // add it
  (this.map[method] = this.map[method] || []).push(route);
  return this;
};
