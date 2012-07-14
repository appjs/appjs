var fs    = require('fs'),
    path  = require('path'),
    mime  = require('mime'),
    Class = require('../utils').Class;


var statusCodes = require('./http.json').statusCodes;
var EMPTY = new Buffer(0);


module.exports = new Class({
  constructor: function Response(callback){
    this.callback = callback;
  },
  send: function send(status, mimetype, data){
    if (arguments.length === 1) {
      if (typeof status === 'number') {
        this.status = status;
      } else {
        this.mimetype = 'text/html';
        this.data = status;
      }
    } else if (arguments.length === 2){
      this.status = status;
      this.data = Buffer.isBuffer(mimetype) ? mimetype : new Buffer(mimetype);
    } else {
      this.status = status;
      this.mimetype = mimetype;
      this.data = Buffer.isBuffer(data) ? data : new Buffer(data);
    }

    this.dispatch();
  },
  dispatch: function dispatch(){
    this.status    || (this.status = 200);
    this.mimetype  || (this.mimetype = 'text/html');
    this.data      || (this.data = EMPTY);
    this.responseCode = statusCodes[this.status];

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

    this.callback(this.status, this.responseCode, this.mimetype, this.data);
  },
  sendFile: function sendFile(status, filepath){
    var self = this;
    if (arguments.length === 2) {
      this.path = path.resolve(filepath);
      this.status = status;
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
});
