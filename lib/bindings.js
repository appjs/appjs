var fs = require('fs'),
    path = require('path');

// normalize node 0.6 and 0.8
fs.existsSync || (fs.existsSync = path.existsSync);



var platforms = {
  win32: function(folder){
    process.env.PATH += ';' + folder;
  }
};

// derive a distributable path from the node version, platform, and arch
function distribPath(){
  return [ '..',
           'compiled',
           process.version.slice(1, 4),
           process.platform,
           process.arch,
           'bindings/'].join('/');
}

// resolve a path relative the directory this .js file is in
function absolutelyRelative(pathname){
  return path.resolve(__dirname, pathname);
}

// find the binary file for a given name
function requireBindings(lib){
  var folder = [ '../build/Release/', // self-compiled release
                 '../build/Debug/',  // self-compiled debug
                  distribPath()      // system-relative pre-compiled
                                     ].map(absolutelyRelative)
                                      .filter(fs.existsSync)[0];
  if (!folder) {
    throw new Error('Unable to locate binaries for '+lib);
  }

  if (process.platform in platforms) {
    platforms[process.platform](folder);
  }

  return require(folder + '/' + lib);
}




module.exports = requireBindings('appjs');
