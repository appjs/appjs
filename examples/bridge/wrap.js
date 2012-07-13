
function initOrigin(){

function isObject(o){
  return o != null && typeof o === 'object' || typeof o === 'function';
}


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

function unwrap(obj){
  if (!isObject(obj)) {
    return obj;
  } else if (obj.__val__) {
    switch (obj.__val__) {
      case '__UND__': return undefined;
      case '__NaN__': return NaN;
      case '__Infinity__': return Infinity;
      case '__-Infinity__': return -Infinity;
      case '__-0__': return -0;
      default: return obj.__val__;
    }
  } else {
    if ('id' in obj && obj.id < items.length) {
      return items[obj.id];
    } else {
      return obj;
    }
  }
}

function wrap(obj){
  if (!isObject(obj)) {
    switch (obj) {
      case undefined: return { __val__: '__UND__' };
      case NaN: return { __val__: '__NaN__' };
      case Infinity: return { __val__: '__Infinity__' };
      case -Infinity: return { __val__: '__-Infinity__' };
      case 0: return { __val__: 1 / obj === -Infinity ? '__-0__' : 0 };
      default: return { __val__: obj };
    }
  } else {
    var id = ids.get(obj);
    if (id === undefined) {
      ids.set(obj, id = items.length);
      items.push(obj);
    }
    return { id: id };
  }
}





var mirror = {
  type: function type(obj){
    return { type: typeof obj };
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
      o.args = o.args ? o.args.map(unwrap) : [];
      o.receiver = unwrap(o.receiver);
      console.log(o);
      return wrap(Function.apply.call(obj, o.receiver, o.args));
    } catch (e) {
      return wrap(e);
    }
  },
  construct: function construct(obj, o){
    try {
      o.args = o.args ? o.args.map(unwrap) : [];
      return wrap(new (Function.bind.apply(obj, [null].concat(o.args))));
    } catch (e) {
      return wrap(e);
    }
  },
};

for (var k in mirror) {
  void function(k){
    pipe.on('mirror-'+k, function(val, res){
      res.result = mirror[k](unwrap(val), val);
      console.log(k, val, res);
    });
  }(k);
}

}



function initClient(win){
function isObject(o){
  return o != null && typeof o === 'object' || typeof o === 'function';
}

var pipe = win ? win : appjs;

var proxies = new WeakMap;
var ids = new WeakMap;
var farObjects = Object.create(null);
var _slice = [].slice;

function FarObject(id, type){
  var self = this;
  this.id = id;
  if (type === 'function') {
    return Proxy.createFunction(this,
      function(){ return self.apply(this, _slice.call(arguments)) },
      function(){ return self.construct(_slice.call(arguments)) }
    );
  } else {
    return Proxy.create(this);
  }
}

function wrap(json){
  if (!isObject(json)) {
    return json;
  } else if (json.__val__) {
    switch (json.__val__) {
      case '__UND__': return undefined;
      case '__NaN__': return NaN;
      case '__Infinity__': return Infinity;
      case '__-Infinity__': return -Infinity;
      case '__-0__': return -0;
      default: return json.__val__;
    }
  } if (json.id in farObjects) {
    return ids.get(farObjects[json.id]);
  } else {
    var type = pipe.send('mirror-type', json).type;
    var proxy = new FarObject(json.id, type);
    proxies.set(proxy, json);
    ids.set(json, proxy);
    farObjects[json.id] = json;
    return proxy;
  }
}

function unwrap(obj){
  if (!isObject(obj)) {
    switch (obj) {
      case undefined: return { __val__: '__UND__' };
      case NaN: return { __val__: '__NaN__' };
      case Infinity: return { __val__: '__Infinity__' };
      case -Infinity: return { __val__: '__-Infinity__' };
      case 0: return { __val__: 1 / obj === -Infinity ? '__-0__' : 0 };
      default: return { __val__: obj };
    }
  } else if (proxies.has(obj)) {
    return proxies.get(obj);
  } else {
    return obj;
  }
}

FarObject.prototype = {
  keys: function keys(){
    return pipe.send('mirror-keys', { id: this.id });
  },
  enumerate: function enumerate(){
    return pipe.send('mirror-enumerate', { id: this.id });
  },
  getOwnPropertyNames: function getOwnPropertyNames(){
    return pipe.send('mirror-properties', { id: this.id });
  },
  get: function get(rcvr, key){
    return wrap(pipe.send('mirror-get', { id: this.id, key: key }));
  },
  set: function set(rcvr, key, value){
    return pipe.send('mirror-set', { id: this.id, key: key, val: unwrap(value) });
  },
  getOwnPropertyDescriptor: function getOwnPropertyDescriptor(key){
    var desc = pipe.send('mirror-describe', { id: this.id, key: key });
    if (desc) {
      desc.configurable = true;
      if (isObject(desc.value)) desc.value = wrap(desc.value);
      if (isObject(desc.get)) desc.get = wrap(desc.get);
      if (isObject(desc.get)) desc.get = wrap(desc.get);
    }
    return desc;
  },
  has: function has(key){
    return pipe.send('mirror-has', { id: this.id, key: key });
  },
  hasOwn: function hasOwn(key){
    return pipe.send('mirror-hasOwn', { id: this.id, key: key });
  },
  delete: function delete_(key){
    pipe.send('mirror-delete', { id: this.id, key: key });
    return true;
  },
  defineProperty: function defineProperty(key, desc){
    desc = unwrap(desc);
    if ('value' in desc) desc.value = unwrap(desc.value);
    if ('get' in desc) desc.get = unwrap(desc.get);
    if ('set' in desc) desc.set = unwrap(desc.set);
    pipe.send('mirror-define', { id: this.id, key: key, desc: desc });
  },
  apply: function apply(receiver, args){
    return wrap(pipe.send('mirror-apply', { id: this.id, receiver: unwrap(receiver), args: args.map(unwrap) }));
  },
  construct: function construct(args){
    return wrap(pipe.send('mirror-construct', { id: this.id, args: args.map(unwrap) }));
  },
};

setTimeout(function(){
  if (window.window) {
    window.node = wrap({ id: 0 });
  } else {
    window.window = wrap({ id: 0 });
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


