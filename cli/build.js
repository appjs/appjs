/**
 * Set the title.
 */

process.title = 'appjs-build'

/**
 * Necessary modules and variables
 */

var fs       = require('fs')
  , spawn    = require('child_process').spawn
  , platform = require('os').platform()
  , arch     = require('os').arch()
  , args     = ['rebuild'];

// TODO: remove me after CEF supports building under OSX as 64bit
if (platform === 'darwin' && arch === 'x64') {
  Array.prototype.unshift.apply(args, ['--arch', 'ia32']);
}

spawn('node-gyp', args, { detached : false, stdio : ['ignore', process.stdout, process.stderr ]});
