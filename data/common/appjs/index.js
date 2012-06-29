var platforms = {
	win32: 'win',
	darwin: 'mac',
	linux: 'linux'
};

module.exports = require('appjs-' + platforms[process.platform]);
