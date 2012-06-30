var platforms = {
	win32: 'win',
	darwin: 'mac',
	linux: 'linux'
};

var version = parseFloat(process.version.replace(/v/,''));

if( version < 0.8 ) {
 throw new Error("AppJS needs node v0.8.x");
}
module.exports = require('appjs-' + platforms[process.platform]);
