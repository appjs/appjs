var path = require('path');

try {
  module.exports = require('./build/Release/appjs.node');
} catch (e) {
  module.exports = require(__dirname + '/../../build/Release/appjs.node');
}

module.exports.initConfig = {
  localsPakPath: __dirname + '/../../app/data/bin/Contents/Resources',
  chromePakPath: path.join(__dirname, 'data/pak/chrome.pak'),
  jsFlags: ' --harmony_proxies --harmony_collections --harmony_scoping'
};
