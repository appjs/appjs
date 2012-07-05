var querystring  = require('querystring'),
    url          = require('url'),
    Class        = require('../utils').Class;

module.exports = new Class({
  constructor: function Request(request){
    var parsed = url.parse(request.url, true);
    this.pathname = request.url;
    this.headers = request.headers;
    this.method = request.method;
    this.pathname = parsed.pathname;
    this.hash = parsed.hash;
    this.params = parsed.query;
    this.data = querystring.parse(request.post);
  },
  param: function param(index){
    return this.params[index];
  },
  post: function post(index){
    return self.data[index];
  },
  get: function get(key){
    return self.headers[key];
  }
});
