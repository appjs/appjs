var platform  = process.platform
  , arch      = process.arch
  , version   = parseFloat(process.version.replace(/v/,''))
  , platforms = {
	win32: 'win',
	darwin: 'mac',
	linux: 'linux'
    };

if( version < 0.8 ) {
  throw new Error("AppJS needs node v0.8.x");
}

if( platform == 'darwin' && arch == 'x64' ) {
  throw new Error("AppJS needs node x64. You can use precompiled packages that include node ia32.");
}

module.exports = require('appjs-' + platforms[platform]);
