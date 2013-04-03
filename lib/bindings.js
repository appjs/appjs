var path = require('path'),
    fs = require('fs');

var platform = process.platform,
    arch     = process.arch,
    versionValues = process.version.substr(1).split('.').map(function (item) { return parseInt(item, 10); });

if (versionValues[0] === 0 && versionValues[1] < 8) {
  throw new Error("AppJS requires Node.js v0.8");
}

if (platform == 'darwin' && arch === 'x64') {
  throw new Error("AppJS requires Node.js 32bit");
}

var bindingsPath = __dirname + '/../bindings/'+platform+'/'+arch+'/appjs.node';

if (!fs.existsSync(bindingsPath)) {
  throw new Error("AppJS .node file not found");
}
bindingsPath = require.resolve(bindingsPath);


var platformPath = path.resolve(__dirname, '..', '..', 'appjs-' + platform + (platform === 'linux' ? '-' + arch : ''));
var libPath = path.resolve(platformPath, 'libs');

if (!fs.existsSync(libPath)) {
  throw new Error("CEF location not found");
}

if (platform === 'win32') {
  process.env.PATH += ';' + libPath;
} else if (platform === 'linux') {
  process.env.LD_LIBRARY_PATH += ':' + libPath;
}

module.exports = require(bindingsPath);
module.exports.platformPath = platformPath;
