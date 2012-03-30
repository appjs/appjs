/**
 * (The MIT License)
 *
 * Copyright (c) 2012 Morteza Milani <mrtz.milani@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **/
 
var archive = require('archive');
var path = require('vpath');


var isWindows = process.platform === 'win32';

var vfs = exports;

vfs.readFileSync = function(filename,encoding){

  var buffer = archive.getEntry(filename);

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

vfs.statSync = function(p){
  p = path.resolve(p);
  return archive.hasEntry(p);
}
