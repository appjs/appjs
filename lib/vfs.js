var archive = require('archive');
var path = require('vpath');


var isWindows = process.platform === 'win32';

var vfs = exports;

vfs.readFileSync = function(filename,encoding){
  
  //@TODO read from virtual fs
  var entry = archive.getEntry(filename);
  var buffer = entry.content;

  if (encoding) buffer = buffer.toString(encoding);
  return buffer;
}

vfs.realpathSync = function(p,cache){
  p = path.resolve(p);
  
  if (cache && Object.prototype.hasOwnProperty.call(cache, p)) {
      return cache[p];
  }

  vfs.statSync(p);
  if (cache) cache[p] = p;
  return p;
}

vfs.statSync = function(path){
  //@TODO return compatible stat object.
  return archive.getEntry(path);
}
