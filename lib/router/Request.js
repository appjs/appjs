var ClientRequest = require('http').ClientRequest,
    querystring   = require('querystring'),
    url           = require('url'),
    inherit       = require('../utils').inherit;

module.exports = Request;

function Request(request){
  var parsed = url.parse(request.url, true);
  this.url = request.url.replace(/^http(s)?:\/\/([^\/]*)\/$/,'');
  this.headers = request.headers;
  this.method = request.method.toLowerCase();
  this.pathname = parsed.pathname;
  this.hash = parsed.hash;
  this.params = parsed.query;
  this.data = this.body = querystring.parse(request.post);
  this.files = request.files;
}

inherit(Request, ClientRequest, [
  function param(index){
    return this.params[index];
  },
  function post(index){
    return this.data[index];
  },
  function get(key){
    return this.headers[key];
  }
]);
