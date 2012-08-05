var fs   = require('fs'),
    path = require('path');

module.exports = {
  decorate: decorate,
  isObject: isObject,
  inherit: inherit,
  constants: constants,
  log: log
};

var isArray = Array.isArray,
    defineProperty = Object.defineProperty,
    getOwnDescriptor = Object.getOwnPropertyDescriptor,
    getOwnNames = Object.getOwnPropertyNames,
    getOwnKeys = Object.keys;


function isObject(o){
  return o != null && typeof o === 'object' || typeof o === 'function';
}

function Definable(desc){
  for (var k in desc)
    this[k] = desc[k];
  this.value = null;
}

Definable.prototype.define = function define(o, key, v){
  this.value = v;
  defineProperty(o, key, this);
  this.value = null;
};


var hiddenDesc = { enumerable: false },
    normalVal = Definable.normal = new Definable({ enumerable: true, configurable: true, writable: true }),
    hiddenVal = Definable.hidden = new Definable({ enumerable: false, configurable: true, writable: true }),
    readonlyVal = Definable.readonly = new Definable({ enumerable: true, configurable: true, writable: false });


function decorate(o){
  var a, b, c, d,
      hidden = arguments[1] === true,
      overwrite = arguments[2] !== false;

  for (a=0; a < arguments.length; a++) {
    if (isArray(b = arguments[a])) {
      for (c = 0; c < b.length; c++) {
        if (typeof b[c] === 'function' && (d = b[c].name)) {
          if (hidden || d === 'toString') {
            hiddenVal.define(o, d, b[c]);
          } else {
            o[d] = b[c];
          }
        }
      }
    } else if (isObject(b)) {
      for (c in b) {
        try {
          if (d = getOwnDescriptor(b, c)) {
            if (d.get || d.set) {
              defineProperty(o, c, d);
            } else {
              o[c] = d.value;
              if (hidden || c === 'toString' || c === 'constructor') {
                defineProperty(o, c, hiddenDesc);
              }
            }
          }
        } catch (e) {}
      }
    }
  }
  return o;
}

function constants(o, p){
  Object.keys(p).forEach(function(key){
    Object.defineProperty(o, key, {
      enumerable: true,
      value: p[key]
    });
  });
}


function inherit(Ctor, Super, props){
  Ctor.prototype = Object.create(Super.prototype);
  decorate(Ctor.prototype, true, props);
  decorate(Ctor.prototype, true, {
    constructor: Ctor
  });
  return Ctor;
}





var logPath = path.resolve(module.parent && module.parent.parent && path.dirname(module.parent.parent.filename) || __dirname);
if (fs.existsSync(logPath + '/appjs.log'))
  logPath = path.resolve(logPath, 'appjs.log');
else if (fs.existsSync(logPath + '/content'))
  logPath = path.resolve(logPath, 'content', 'appjs.log');
else
  logPath = path.resolve(logPath, 'appjs.log');


function isError(o){
  if (!o || typeof o !== 'object') return false;
  if (Error.prototype.isPrototypeOf(o)) return true;
  if (/Error$/.test(o.name)) return true;
  if ('stack' in o) return true;
  return false;
}

function log(path, type, context, msg){
  var err;
  if (isError(type)) err = type, type = null;
  else if (isError(context)) err = context, context = null;
  else if (isError(msg)) err = msg, msg = null;
  if (err) {
    if (!msg) msg = err.stack || err.message;
    if (!type) type = err.message || err.name;
  }
  var out = '';
  if (type) out = type + ': ';
  if (msg) out += msg;
  if (context && typeof context === 'object') {
    try { context = JSON.stringify(context) }
    catch (e) { context += '' }
    out += '[ ' + context + ' ]';
  }
  if (out) {
    var file = path || logPath;
    try { fs.writeFileSync(logPath, out) }
    catch (e) { console.log(e, out) }
  }
}
