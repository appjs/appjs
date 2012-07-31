var fs = require('fs'),
    path = require('path');

module.exports = {
  Class: Class,
  decorate: decorate,
  isObject: isObject,
  inherit: inherit,
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


function Class(proto){
  var Ctor = proto.constructor || (proto.constructor = Object);
  var Super = proto.super || Object;
  this.__proto__ = Super.prototype;
  Ctor.__proto__ = Super;
  Ctor.prototype = this;
  getOwnNames(proto).forEach(function(key){
    if (key !== 'super') {
      var desc = getOwnDescriptor(proto, key);
      if (key[0] === '_' || key === 'constructor') {
        desc.enumerable = false;
        key[0] === '_' && (key = key.slice(1));
      }
      defineProperty(this, key, desc);
    }
  }, this);
  return Ctor;
}


function decorate(o){
  var a, b, c, d,
      hidden = arguments[1] === true,
      overwrite = arguments[2] !== false;

  for (a=0; a < arguments.length; a++) {
    if (isArray(b = arguments[a])) {
      for (c = 0; c < b.length; c++) {
        if (typeof b[c] === 'function' && b[c].name) {
          if (hidden) {
            hiddenVal.define(o, b[c].name, b[c]);
          } else {
            o[b[c].name] = b[c];
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
              if (hidden) {
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


function inherit(Ctor, Super, props){
  Ctor.prototype = Object.create(Super.prototype);
  decorate(Ctor.prototype, true, props);
  decorate(Ctor.prototype, true, {
    constructor: Ctor
  });
  return Ctor;
}



var Icons = module.exports.Icons = function(){
  function Icons(o){
    if (typeof o === 'string') {
      this.setRoot(o);
    } else if (isObject(o)) {
      this.setObject(o);
    } else if (Array.isArray(o)) {
      this.setArray(o);
    }
  }

  var namedSizes = ['smaller', 'small', 'largel', 'larger'],
      numberSizes = [16, 32, 64, 128],
      sizeMap = {
        16: 'smaller',
        32: 'small',
        64: 'large',
        128: 'larger',
        smaller: 'smaller',
        small: 'small',
        large: 'large',
        larger: 'larger'
      };

  decorate(Icons.prototype, [
    function setRoot(root){
      if (fs.existsSync(root + '/small.png')) {
        var sizes = namedSizes;
      } else if (fs.existsSync(root + '/16.png')) {
        var sizes = numberSizes;
      } else {
        return this;
      }
      sizes.forEach(function(size){
        this[sizeMap[size]] = path.resolve(root, size+'.png');
      }, this);
      this.resolve();
      return this;
    },
    function setObject(obj){
      for (var k in obj) {
        if (sizeMap.hasOwnProperty(k)) {
          this[sizeMap[k]] = obj[k];
        }
      }
      this.resolve();
      return this;
    },
    function setArray(array){
      array.forEach(function(item){
        if (typeof item === 'string' && /\.png$/.test(item)) {
          var name = path.basename(item).slice(0, -4);
          if (sizeMap.hasOwnProperty(name)) {
            this[sizeMap[name]] = item;
          }
        }
      }, this);
      this.resolve();
      return this;
    },
    function resolve(){
      Object.keys(this).forEach(function(key){
        this[key] = path.resolve(this[key]);
        if (!fs.existsSync(this[key])) {
          delete this[key];
        }
      }, this);
      return this;
    }
  ]);

  return Icons;
}();




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


var names = module.exports.names = [
  'alpha', 'bravo', 'charlie', 'delta', 'echo', 'foxtrot', 'golf',
  'hotel', 'india', 'juliet', 'kilo', 'lima', 'mike', 'november',
  'oscar', 'papa', 'quebec', 'romeo', 'sierra', 'tango', 'uniform',
  'victor', 'whiskey', 'x-ray', 'yankee', 'zulu'
];

names.index = 0;
names.suffix = '';
