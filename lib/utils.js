module.exports = {
  Class: Class
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
