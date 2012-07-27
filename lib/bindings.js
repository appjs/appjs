var platform  = process.platform,
    arch      = process.arch,
    version   = parseFloat(process.version.replace(/v/,''));

if ( version < 0.8 ) {
  throw new Error("AppJS needs node v0.8.x");
}

if ( platform == 'darwin' && arch == 'x64' ) {
  throw new Error("AppJS needs node x64. You can use precompiled packages that include node ia32.");
}

if( platform == "linux" ) {
	module.exports = require('appjs-' + platform + '-' + arch);
} else {
	module.exports = require('appjs-' + platform);
}