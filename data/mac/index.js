var path = require('path');

module.exports = require('./build/Release/appjs.node');

module.exports.initConfig = {
  localesPakPath: path.resolve(__dirname, 'data/pak/locales'),
  chromePakPath: path.resolve(__dirname, 'data/pak/chrome.pak'),
  jsFlags: ' --harmony_proxies --harmony_collections --harmony_scoping'
};
