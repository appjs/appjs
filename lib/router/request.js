var querystring  = require('querystring'),
    url          = require('url'),
    Class        = require('../utils').Class;

module.exports = new Class({
  __proto__: require('http').ClientRequest.prototype,
  constructor: function Request(request){
    var parsed = url.parse(request.url, true);
    this.url = request.url.replace(/^http(s)?:\/\/([^\/]*)\/$/,'');
    this.headers = request.headers;
    this.method = request.method.toLowerCase();
    this.pathname = parsed.pathname;
    this.hash = parsed.hash;
    this.params = parsed.query;
    this.data = this.body = querystring.parse(request.post);
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
