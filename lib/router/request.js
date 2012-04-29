var querystring = require('querystring');
var url = require('url');

function Request(req) {

  var parsedURL = url.parse(req.url,true);

  this.url = req.url;
  this.headers = req.headers;
  this.method = req.method;
  this.pathname = parsedURL.pathname;
  this.hash = parsedURL.hash;
  this.params = parsedURL.query;
  this.postData = querystring.parse(req.post);

  var self = this;

  this.param = function(index) {
    return self.params[index];
  }

  this.post = function(index) {
    return self.postData[index];
  }

  this.get = function(key) {
    return self.headers[key];
  }

  return this;
}

module.exports = Request;