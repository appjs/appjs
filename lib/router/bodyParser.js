/*!
 * Influenced by Connect - bodyParser
 * Copyright(c) 2010 Sencha Inc.
 * Copyright(c) 2011 TJ Holowaychuk
 * Copyright(c) 2012 AppJS
 * MIT Licensed
 */

/**
 * Module dependencies.
 */

var qs = require('querystring');

/**
 * Extract the mime type from the given request's
 * _Content-Type_ header.
 *
 * @param  {IncomingMessage} req
 * @return {String}
 * @api private
 */

function mime(req) {
  var str = req.headers['Content-Type'] || '';
  return str.split(';')[0];
}

exports.parse = function Parse(req) {
  if (req.body) return;
  req.body = {};

  if ('get' == req.method || 'head' == req.method) return;
  var parser = exports.parsers[mime(req)];
  if (parser) {
    parser(req);
  }
}

/**
 * Parsers.
 */

exports.parsers = {};

/**
 * Parse application/x-www-form-urlencoded.
 */

exports.parsers['application/x-www-form-urlencoded'] = function(req){
  try {
    req.post = req.body = req.post.length
      ? qs.parse(req.post)
      : {};
  } catch (err){
    // TODO ?
    console.log("[AppJS] couldn't parse x-www-form-urlencoded");
  }
};

/**
 * Parse application/json.
 */

exports.parse['application/json'] = function(req){
  try {
    req.post = req.body = req.post.length
      ? JSON.parse(req.post)
      : {};
  } catch (err){
    console.log("[AppJS] couldn't parse application/json");
  }
};

/**
 * Parse multipart/form-data.
 *
 * Note: Is it OK to use this function with the assumption
 * that CEF pre-parse files and extract them from post data?
 */

exports.parse['multipart/form-data'] = function(req){
  try {
    req.post = req.body = req.post.length
      ? qs.parse(req.post)
      : {};
  } catch (err){
    // TODO ?
    console.log("[AppJS] couldn't parse form-data");
  }
};
