var decorate = require('./utils').decorate;

var _hasOwn = Object.prototype.hasOwnProperty,
    _apply = Function.prototype.apply,
    _bind = Function.prototype.bind;

module.exports = {
  Ephemeral: Ephemeral,
  MirrorHandler: MirrorHandler,
  ReferenceType: ReferenceType
};


var ephemerals = new WeakMap;

function Ephemeral(target, data){
  this.handler = new MirrorHandler(target);
  Object.keys(Object(data)).forEach(function(key){
    this[key] = data[key];
  }, this);
  var proxy = Proxy.create(this.handler, target.__proto__);
  ephemerals.set(proxy, this);
  return proxy;
}

Ephemeral.unwrap = function unwrap(target){
  return ephemerals.get(target);
};

decorate(Ephemeral.prototype, [
  function retarget(target){
    this.handler.target = target;
  },
  function morph(handler, data){
    this.handler.__proto__ = handler;
    Object.keys(Object(data)).forEach(function(key){
      this[key] = data[key];
    }, this.handler);
  },
  function unmorph(){
    this.handler.__proto__ = MirrorHandler.prototype;
  }
]);



function MirrorHandler(target){
  this.target = target;
}

decorate(MirrorHandler.prototype, [
  function keys(){
    return Object.keys(this.target);
  },
  function enumerate(){
    var i=0, k=[];
    for (k[i++] in this.target);
    return k;
  },
  function getOwnPropertyNames(){
    return Object.getOwnPropertyNames(this.target);
  },
  function get(rcvr, key){
    return this.target[key];
  },
  function set(rcvr, key, value){
    this.target[key] = value;
    return true;
  },
  function has(key){
    return key in this.target;
  },
  function hasOwn(key){
    return _hasOwn.call(this.target, key);
  },
  function delet\u0065(key){
    delete this.target[key];
    return true;
  },
  function defineProperty(key, desc){
    Object.defineProperty(this.target, key, desc);
    return true;
  },
  function getOwnPropertyDescriptor(key){
    var desc = Object.getOwnPropertyDescriptor(this.target, key);
    desc && (desc.configurable = true);
    return desc;
  },
  function apply(rcvr, args){
    return _apply.call(this.target, rcvr, args);
  },
  function construct(args){
    return new (_bind.apply(this.target, [null].concat(args)));
  }
]);



var references = new WeakMap;

function ReferenceType(keys, accessors){
  this.accessors = Object.create(null);
  this.refKeys = Object.create(null);

  if (Array.isArray(keys)) {
    this.names = keys;
    keys.forEach(function(key){ this[key] = key }, this.refKeys);
  } else {
    this.names = Object.keys(keys);
    this.names.forEach(function(key){ this[key] = keys[key] }, this.refKeys);
  }

  if (accessors) {
    var accessorKeys = Object.keys(accessors);
    this.names = this.names.concat(accessorKeys);
    accessorKeys.forEach(function(key){
      this[key] = Object.getOwnPropertyDescriptor(accessors, key);
    }, this.accessors);
  }

  function ReferenceHandler(reference, target){
    this.reference = reference;
    this.target = target;
  }

  ReferenceHandler.prototype = this;

  return function Reference(reference){
    var proxy = Proxy.create(new ReferenceHandler(reference, this), Reference.prototype);
    references.set(proxy, reference);
    return proxy;
  };
}

ReferenceType.unwrap = function unwrap(target){
  return references.get(target);
};


decorate(ReferenceType.prototype, [
  function keys(){
    return this.names.concat(Object.keys(this.target));
  },
  function enumerate(){
    var i = this.names.length, k = this.names.slice();
    for (k[i++] in this.target);
    return k;
  },
  function getOwnPropertyNames(){
    return this.names.concat(Object.getOwnPropertyNames(this.target));
  },
   function get(rcvr, key){
    if (key === '__proto__') {
      return this.target.__proto__;
    } else if (key in this.refKeys) {
      return this.reference[this.refKeys[key]];
    } else if (key in this.accessors) {
      return this.accessors[key].get.call(this.reference);
    } else {
      return this.target[key];
    }
  },
  function set(rcvr, key, value){
    if (key in this.refKeys) {
      this.reference[this.refKeys[key]] = value;
    } else if (key in this.accessors) {
      this.accessors[key].set.call(this.reference, value);
    } else {
      this.target[key] = value;
    }
    return true;
  },
  function has(key){
    return key in this.refKeys || key in this.accessors || key in this.target;
  },
  function hasOwn(key){
    return key in this.refKeys || key in this.accessors || _hasOwn.call(this.target, key);
  },
  function delet\u0065(key){
    if (key in this.refKeys) {
      delete this.reference[this.refKeys[key]];
    } else {
      delete this.target[key];
    }
    return true;
  },
  function defineProperty(key, desc){
    if (key in this.refKeys) {
      Object.defineProperty(this.reference, this.refKeys[key], desc);
    } else if (key in this.accessors) {
      this.accessors[key].set.call(this.reference, desc.value);
    } else {
      Object.defineProperty(this.target, key, desc);
    }
    return true;
  },
  function getOwnPropertyDescriptor(key){
    if (key in this.refKeys) {
      var desc = Object.getOwnPropertyDescriptor(this.reference, this.refKeys[key]);
    } else if (key in this.accessors) {
      return {
        enumerable: true, configurable: true, writable: true,
        value: this.accessors[key].get.call(this.reference)
      };
    } else {
      var desc = Object.getOwnPropertyDescriptor(this.target, key);
    }
    desc && (desc.configurable = true);
    return desc;
  },
  function apply(rcvr, args){
    return _apply.call(this.reference, rcvr, args);
  },
  function construct(args){
    return new (_bind.apply(this.reference, [null].concat(args)));
  }
]);
