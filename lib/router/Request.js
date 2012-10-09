var ClientRequest = require('http').ClientRequest,
    url           = require('url'),
    inherit       = require('../utils').inherit;

module.exports = Request;

function Request(request){
  var decodedURL = decodeURIComponent(request.url);
  var parsed = url.parse(decodedURL, true);
  this.url = decodedURL;
  this.headers = request.headers;
  this.method = request.method.toLowerCase();
  this.pathname = parsed.pathname;
  this.hash = parsed.hash;
  this.params = parsed.query;
  this.data = this.body = request.post;
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
