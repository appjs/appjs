/**
 * (The MIT License)
 *
 * Copyright (c) 2012 Morteza Milani <mrtz.milani@gmail.com> and other 
 * AppJS contributors.
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
 
var inherits = require('util').inherits;

function MyError(msg) {
  this.name = 'Appjs Error';
  this.message = msg;
  Error.call(this, msg);
  Error.captureStackTrace(this, arguments.callee);
}

inherits(MyError, Error);

var l = 1, COLOR = [], LEVEL = {
  verbose : l++,
  info:     l++,
  warn:     l++,
  error:    l++,
  success:  l++
};

COLOR[LEVEL.verbose] = 34;
COLOR[LEVEL.info] = 32;
COLOR[LEVEL.warn] = 33;
COLOR[LEVEL.error] = 31;
COLOR[LEVEL.success] = 32;

function colorize (msg, color) {
  if(!color) color = COLOR[LEVEL.info];
  return msg ? "\033["+color+"m"+msg+"\033[0m" : "";
}

var logLevel = LEVEL['info'];

var log = function(msg,level){

  if(!level || !LEVEL[level]){
    level = 'info';
  }

  if (msg instanceof MyError) {

    msg = msg.toString();
    level = 'error';

  } else if (msg instanceof Error){

    msg = msg.stack || msg.toString();
    level = 'error';

  }else {

    msg = level + ': ' + msg;

  }
  
  if(LEVEL[level] < logLevel) return;

  console.log(colorize(msg,COLOR[LEVEL[level]]));
  
}

log.LEVELS = LEVEL;

log.setLevel = function(level){
  logLevel = LEVEL[level];
}

Object.keys(LEVEL).forEach(function (l) {
  log[l] = l;
});

exports.abort = function(error){
  if(error instanceof Error){
    throw error;
  } else {
    throw new MyError(error);
  }
}

exports.errorHandler = function(e){
  //msg = msg.toString();
  log(e);
}

exports.log = log;
