var binaryPath = require.resolve('./build/Release/appjs.node');
process.env.PATH += ';' + binaryPath;
module.exports = require(binaryPath);
module.exports.locals = require('path').resolve(__dirname, 'data', 'pak');
