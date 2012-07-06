var querystring  = require('querystring'),
    url          = require('url'),
    Class        = require('../utils').Class;

module.exports = new Class({
  constructor: function Request(request){
    var parsed = url.parse(request.url, true);
    this.headers = request.headers;
    this.method = request.method.toLowerCase();
    this.pathname = parsed.pathname;
    this.hash = parsed.hash;
    this.params = parsed.query;
    this.data = querystring.parse(request.post);
  },
  param: function param(index){
    return this.params[index];
  },
  post: function post(index){
    return this.data[index];
  },
  get: function get(key){
    return this.headers[key];
  }
});
