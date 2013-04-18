var path = require('path')
  , fs = require('fs')
;

var platform = process.platform
  , arch     = process.arch
  , platformPath = path.resolve(__dirname, '..', '..', 'appjs-' + platform + (platform === 'linux' ? '-' + arch : ''))
;

module.exports = process.binding('appjs');
module.exports.platformPath = platformPath;
