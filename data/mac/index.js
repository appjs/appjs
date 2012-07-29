var path = require('path');

module.exports = require('./build/Release/appjs.node');

module.exports.initConfig = {
  LocalesPakPath: path.resolve(__dirname, 'data/pak/locales'),
  ChromePakPath:  path.resolve(__dirname, 'data/pak/chrome.pak')
};
