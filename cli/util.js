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
