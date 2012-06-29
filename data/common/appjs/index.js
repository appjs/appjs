var platforms = {
	win32: 'win',
	mac: 'mac',
	linux: 'linux'
};

module.exports = require('appjs-' + platforms[process.platform]);
