var path = require('path');
var binaryPath = require.resolve('./build/Release/appjs.node');

process.env.PATH += ';' + binaryPath;

module.exports = require(binaryPath);

module.exports.initConfig = {
  LocalesPakPath: path.resolve(__dirname, 'data/pak/locales'),
  ChromePakPath:  path.resolve(__dirname, 'data/pak/chrome.pak'),
};
