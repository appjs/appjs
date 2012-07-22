var platform  = { win32: 'win', darwin: 'mac', linux: 'linux' }[process.platform],
    arch      = process.arch,
    version   = parseFloat(process.version.replace(/v/,'')),
    path      = require('path');

if ( version < 0.8 ) {
  throw new Error("AppJS needs node v0.8.x");
}

if ( platform == 'mac' && arch == 'x64' ) {
  throw new Error("AppJS needs node x64. You can use precompiled packages that include node ia32.");
}

try {
  module.exports = require('appjs-' + platform);

} catch (e) {
  module.exports = require(path.join('..', 'data', platform));
}
console.log(module.exports);
