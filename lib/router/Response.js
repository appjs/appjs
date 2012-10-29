var ServerResponse = require('http').ServerResponse,
    STATUS_CODES   = require('http').STATUS_CODES,
    fs             = require('fs'),
    path           = require('path'),
    url            = require('url'),
    mime           = require('mime'),
    inherit        = require('../utils').inherit,
    decorate       = require('../utils').decorate;

var EMPTY = new Buffer(0);

var appjsVersion = 'AppJS' + require('../../package.json').version;



module.exports = Response;

function Response(request, callback){
  this.callback = callback;
  this.request = request;
  this.headers = baseHeaders.child();
}

inherit(Response, ServerResponse, [

  // we can set the status of the response
  // Rename the 'status' property in order to use this function
  function status(status){
    this.statusCode = status;
    return this;
  },
  function end(data){
    // be sure all parameters will be passed to 'send' method
    return this.send.apply(this, arguments);
  },
  function send(status, mimetype, data){
    // rename the 'status' property in order to use 'status' function
    if (arguments.length === 1) {
      if (typeof status === 'number') {
        this.statusCode = status;
      } else {
        this.mimetype = 'text/html';
        this.data = status;
      }
    } else if (arguments.length === 2){
      this.statusCode = status;
      this.data = Buffer.isBuffer(mimetype) ? mimetype : new Buffer(mimetype);
    } else {
      this.statusCode = status;
      this.mimetype = mimetype;
      this.data = Buffer.isBuffer(data) ? data : new Buffer(data);
    }

    this.dispatch();
  },
  function dispatch(){
    this.statusCode    || (this.statusCode = 200);
    this.mimetype  || (this.mimetype = 'text/html');
    this.data      || (this.data = EMPTY);
    this.responseCode = STATUS_CODES[this.statusCode];

    if (!Buffer.isBuffer(this.data)) {
      if (this.data !== null && typeof this.data === 'object') {
        this.mimetype = 'application/json';
        this.data = new Buffer(JSON.stringify(this.data));
      } else if (typeof this.data === 'string' || typeof this.data === 'number') {
        this.data = new Buffer(this.data);
      } else {
        this.data = EMPTY;
      }
    }

    this.headers.setHeader('Content-Length', this.data.length);
    this.headers.setHeader('Content-Type', this.mimetype);
    this.callback(this.statusCode, this.responseCode, this.mimetype, this.headers.toArrays(), this.data);
  },
  function redirect(destination){
    this.statusCode = 302;
    this.setHeader('Location', url.resolve(this.request.headers.Referer, destination));
    this.dispatch();
    return this;
  },
  function setHeader(name, header){
    this.headers.setHeader(name, header);
  },
  function sendFile(status, filepath){
    var self = this;
    if (arguments.length === 2) {
      this.path = path.resolve(filepath);
      this.statusCode = status;
    } else {
      this.path = path.resolve(status);
    }

    fs.stat(this.path, function(err, stat){
      if (err || !stat.isFile()) {
        self.send(404);
      } else {
        self.mimetype = mime.lookup(self.path);
        fs.readFile(self.path, function(err, buffer){
          if (err) {
            self.send(500);
          } else {
            self.data = buffer;
            self.dispatch();
          }
        });
      }
    });
  }
]);

function Headers(init){
  if (init instanceof Headers) {
    this.inheritHeaders(init);
  } else {
    this.names = Object.create(null);
    this.headers = Object.create(null);
    if (init) {
      this.setHeaders(init);
    }
  }
}

decorate(Headers.prototype, true, [
  function setHeader(name, value){
    var key = name.toLowerCase();
    this.names[key] = name;
    this.headers[key] = value;
  },
  function setHeaders(source){
    if (source instanceof Headers) {
      source.listHeaders().forEach(function(name){
        var key = name.toLowerCase();
        this.names[key] = name;
        this.headers[key] = source.headers[key];
      }, this);
    } else {
      for (var k in Object(source)) {
        this.setHeader(k, source[k]);
      }
    }
  },
  function inheritHeaders(source){
    if (source instanceof Headers) {
      if (!this.names || !this.listHeaders().length) {
        this.names = Object.create(source.names);
        this.headers = Object.create(source.headers);
      } else {
        this.names.__proto__ = source.names;
        this.headers.__proto__ = source.headers;
      }
    } else {
      // throw?
    }
  },
  function getHeader(name){
    return this.headers[name.toLowerCase()];
  },
  function removeHeader(name){
    var key = name.toLowerCase();
    delete this.names[key];
    delete this.headers[key];
  },
  function listHeaders(){
    var i=0, k=[];
    for (k[i++] in this.names);
    return k;
  },
  function toArrays(){
    var out = { names: [], headers: [] }
    for (var k in this.names) {
      out.names.push(this.names[k]);
      out.headers.push(this.headers[k]);
    }
    return out;
  },
  function toObject(){
    var out = {};
    this.listHeaders().forEach(function(key){
      out[this.names[key]] = this.headers[key];
    }, this);
    return out;
  },
  function child(){
    return new Headers(this);
  }
]);



var baseHeaders = new Headers({
  'Connection': 'Keep-Alive',
  'Keep-Alive': 'timeout=3, max=100',
  'Server': appjsVersion
});

