var ServerResponse = require('http').ServerResponse,
    STATUS_CODES   = require('http').STATUS_CODES,
    fs             = require('fs'),
    path           = require('path'),
    mime           = require('mime'),
    inherit        = require('../utils').inherit;

var EMPTY = new Buffer(0);


module.exports = Response;

function Response(callback){
  this.callback = callback;
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

    this.callback(this.statusCode, this.responseCode, this.mimetype, this.data);
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
        self.mimetype = mime.lookup(this.path);
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
