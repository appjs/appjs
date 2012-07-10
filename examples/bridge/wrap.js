
function initOrigin(){
var ids = new WeakMap;
if (typeof global === 'undefined') {
  var items = [window];
  var pipe = appjs;
  ids.set(window, 0);
} else {
  var items = [global];
  var pipe = window;
  ids.set(global, 0);
}

function unwrap(o){
  if (o == null) return o;
  if (typeof o === 'object') {
    if ('id' in o && o.id < items.length) {
      return items[o.id];
    } else if ('val' in o) {
      return o.val;
    } else {
      return o;
    }
  } else {
    return o;
  }
}

function wrap(obj){
  if (obj == null || typeof obj !== 'object' && typeof obj !== 'function') {
    return { val: obj };
  }
  var id = ids.get(obj);
  if (id === undefined) {
    ids.set(obj, id = items.length);
    items.push(obj);
  }
  return { id: id };
}




var mirror = {
  type: function type(id){
    return { type: typeof unwrap({ id: id }) };
  },
  id: function id(path){
    path = path.split('/');
    var root = unwrap({ id: path[0] }), curr = root;
    for (var i=1; i < path.length; i++) {
      if (!(path[i] in curr)) return { __notfound: path.slice(0, i).join('/') };
      curr = curr[path[i]];
    }
    var ret = wrap(curr);
    ret.type = curr === null ? 'null' : typeof curr;
    return ret;
  },
  keys: function keys(obj, o){
    return Object.keys(obj);
  },
  enumerate: function enumerate(obj, o){
    var i=0,k=[];
    for (k[i++] in obj);
    return k;
  },
  properties: function properties(obj, o){
    return Object.getOwnPropertyNames(obj);
  },
  get: function get(obj, o){
    return wrap(obj[o.key]);
  },
  set: function set(obj, o){
    obj[o.key] = unwrap(o);
    return true;
  },
  has: function has(obj, o){
    return o.key in obj;
  },
  hasOwn: function hasOwn(obj, o){
    return {}.hasOwnProperty.call(obj, o.key);
  },
  delete: function delete_(obj, o){
    delete obj[o.key];
    return true;
  },
  define: function define(obj, o){
    if ('value' in o.desc) o.desc.value = unwrap(o.desc.value);
    if ('get' in o.desc) o.desc.get = unwrap(o.desc.get);
    if ('set' in o.desc) o.desc.set = unwrap(o.desc.set);
    Object.defineProperty(obj, o.key, o.desc);
    return true;
  },
  describe: function describe(obj, o){
    var desc = Object.getOwnPropertyDescriptor(obj, o.key);
    if (desc) {
      if ('value' in desc) desc.value = wrap(desc.value);
      if ('get' in desc) desc.get = wrap(desc.get);
      if ('set' in desc) desc.set = wrap(desc.set);
    }
    return desc;
  },
  apply: function apply(obj, o){
    try {
      o.args = o.args || [];
      return wrap(Function.apply.call(obj, unwrap(o.receiver), o.args.map(unwrap)));
    } catch (e) {
      return wrap(e);
    }
  },
  construct: function construct(obj, o){
    try {
      o.args = o.args || [];
      return wrap(new (Function.bind.apply(obj, [null].concat(o.args.map(unwrap)))));
    } catch (e) {
      return wrap(e);
    }
  },
};

if (pipe === window) {
  for (var k in mirror) {
    void function(){
      var func, type = k;
      if (type === 'id') func = function(val, res){
        res.result = mirror.id(val);
      }
      else func = function(val, res){
        res.result = mirror[type](unwrap(val), val);
      }
      pipe.on('mirror-'+type, func);
    }();
  }
} else {
  for (var k in mirror) {
    void function(){
      var func, type = k;
      if (type === 'id') func = function(val){
        return mirror.id(val);
      }
      else func = function(val){
        return mirror[type](unwrap(val), val);
      }
      pipe.on('mirror-'+type, func);
    }();
  }
}
}



function initClient(win){

var mirrors = {};

var pipe = win ? win : appjs;

function Mirror(path){
  var ret = pipe.send('mirror-id', path);
  this.id = ret.id;
  this.type = ret.type;
  if (this.id in mirrors) {
    return mirrors[this.id];
  }
}

function getId(id){
  if (!(id in mirrors)) {
    mirrors[id] = Object.create(Mirror.prototype);
    mirrors[id].id = id;
    var type = pipe.send('mirror-type', id);
    if (type && type.type) {
      mirrors[id].type = type.type;
    }
  }
  return mirrors[id];
}

Mirror.create = function create(o){
  if (o == null) return o;
  if (typeof o === 'object') {
    if ('id' in o) {
      return getId(o.id);
    } else if ('val' in o) {
      return o.val;
    } else {
      return o;
    }
  }
  return o;
}

Mirror.prototype = {
  constructor: Mirror,
  keys: function keys(){
    return pipe.send('mirror-keys', { id: this.id });
  },
  enumerate: function enumerate(){
    return pipe.send('mirror-enumerate', { id: this.id });
  },
  properties: function properties(){
    return pipe.send('mirror-properties', { id: this.id });
  },
  get: function get(key){
    return Mirror.create(pipe.send('mirror-get', { id: this.id, key: key }));
  },
  set: function set(key, value){
    return pipe.send('mirror-set', { id: this.id, key: key, val: value });
  },
  has: function has(key){
    return pipe.send('mirror-has', { id: this.id, key: key });
  },
  hasOwn: function hasOwn(key){
    return pipe.send('mirror-hasOwn', { id: this.id, key: key });
  },
  delete: function delete_(key){
    return pipe.send('mirror-delete', { id: this.id, key: key });
  },
  define: function define(key, desc){
    return pipe.send('mirror-define', { id: this.id, key: key, desc: desc });
  },
  describe: function describe(key){
    var result = pipe.send('mirror-describe', { id: this.id, key: key });
    if (result) {
      if ('value' in result) result.value = Mirror.create(result.value);
      if ('get' in result) result.get = Mirror.create(result.get);
      if ('set' in result) result.set = Mirror.create(result.set);
    }
    return result;
  },
  apply: function apply(receiver, args){
    return Mirror.create(pipe.send('mirror-apply', { id: this.id, receiver: receiver, args: args.map(Mirror.create) }));
  },
  construct: function construct(args){
    return Mirror.create(pipe.send('mirror-construct', { id: this.id, args: args.map(Mirror.create) }));
  },
};



var farobjects = new WeakMap;
var proxies = new WeakMap;

function isObject(o){
  return o != null && typeof o === 'object' || typeof o === 'function';
}

function FarObject(mirror){
  if (mirror instanceof Mirror && farobjects.has(mirror)) {
    return farobjects.get(mirror);
  }
  if (isObject(mirror) && proxies.has(mirror))
      return mirror;
  this.mirror = mirror;
  if (mirror.type === 'function') {
    var proxy = Proxy.createFunction(this,
      function(){
        var rcvr = isObject(this) ? proxies.get(this) : { val: this };
        return FarObject.wrap(mirror.apply(rcvr, [].slice.call(arguments)));
      },
      function(){
        return FarObject.wrap(mirror.construct([].slice.call(arguments)));
      }
    );
  } else {
    var proxy = Proxy.create(this);
  }
  farobjects.set(mirror, proxy);
  proxies.set(proxy, mirror);
  return proxy;
}

FarObject.wrap = function wrap(o){
  if (o instanceof Mirror) {
    return new FarObject(o);
  } else if (o && typeof o === 'object' && o.value instanceof Mirror) {
    if (o.set instanceof Mirror) o.set = new FarObject(o.set);
    if (o.get instanceof Mirror) o.get = new FarObject(o.get);
    if (o.value instanceof Mirror) o.value = new FarObject(o.value);
    return o;
  } else {
    return o;
  }
}

FarObject.prototype = {
  get: function get(rcvr, key){
    return FarObject.wrap(this.mirror.get(key));
  },
  set: function set(rcvr, key, value){
    return this.mirror.set(key, value);
  },
  keys: function keys(){
    return this.mirror.keys();
  },
  enumerate: function enumerate(){
    return this.mirror.enumerate();
  },
  getOwnPropertyNames: function getOwnPropertyNames(){
    return this.mirror.properties();
  },
  getOwnPropertyDescriptor: function getOwnPropertyDescriptor(key){
    var desc = this.mirror.describe(key);
    var self = this;
    if (desc) {
      desc.configurable = true;
      if ('get' in desc) {
        desc.get = function(){ return FarObject.wrap(self.mirror.get(key)) }
      }
      if ('set' in desc) {
        desc.set = function(v){ return self.mirror.set(key, v) }
      }
      if ('value' in desc) {
        desc.value = FarObject.wrap(Mirror.create(desc.value));
      }
    }
    return desc;
  },
  has: function has(key){
    return this.mirror.has(key);
  },
  delete: function delete_(key){
    return this.mirror.delete(key);
  },
  hasOwn: function has(key){
    return this.mirror.hasOwn(key);
  },
  defineProperty: function defineProperty(key, desc){
    return this.mirror.define(key, desc);
  }
};

setTimeout(function(){
  if (window.window) {
    window.node = FarObject.wrap(Mirror.create({ id: 0 }));
  } else {
    window.window = FarObject.wrap(Mirror.create({ id: 0 }));
  }
}, 1000);
}

var app = module.exports = require('appjs').init();

app.get('/',function(req,res,next){
  res.send(200,"Hello World!");
});

var window = app.createWindow("http://appjs/", {
  width: 640,
  height: 460,
  left: -1,
  top: -1,
  autoResize: false,
  resizable: false,
  showChrome: true,
  opacity: 1,
  fullscreen: false,
  showResizeGrip: false,
  disableSecurity: true,
});


window.on("create",function(){
  console.log("Window Created");
  this.show();
  this.openDevTools();
});

window.on("ready",function(){
  console.log("Page Loaded");
  initOrigin(this);
  initClient(this);
  this.runInBrowser(initOrigin);
  this.runInBrowser(initClient);
});

window.on("close",function(){
  console.log("Window Closed");
});


