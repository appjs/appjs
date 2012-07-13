module.exports = {
  Class: Class,
  extend: extend
};

function Class(proto){
  var Ctor = proto.constructor || (proto.constructor = Object);
  var Super = proto.super || Object;
  this.__proto__ = Super.prototype;
  Ctor.__proto__ = Super;
  Ctor.prototype = this;
  Object.getOwnPropertyNames(proto).forEach(function(key){
    if (key !== 'super') {
      var desc = Object.getOwnPropertyDescriptor(proto, key);
      if (key[0] === '_') {
        desc.enumerable = false;
        key = key.slice(1);
      } else if (key === 'constructor') {
        desc.enumerable = false;
      }

      Object.defineProperty(this, key, desc);
    }
  }, this);
  return Ctor;
}


function extend(obj){
  var desc = { enumerable: false, configurable: true, writable: true };
  for (var i=1; i < arguments.length; i++) {
    Object.keys(arguments[i] || {}).forEach(function(key){
      desc.value = this[key];
      Object.defineProperty(obj, key, desc);
    }, arguments[i]);
  }
  return obj;
}
