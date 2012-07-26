var path = require('path');
var binaryPath = require.resolve('./build/Release/appjs.node');

process.env.PATH += ';' + binaryPath;

module.exports = require(binaryPath);

module.exports.initConfig = {
  localesPakPath: path.resolve(__dirname, 'data/pak/locales'),
  chromePakPath: path.resolve(__dirname, 'data/pak/chrome.pak'),
  jsFlags: ' --harmony_proxies --harmony_collections --harmony_scoping'
};
