
function initPipe(){
  var FP         = Function.prototype,
      bindbind   = FP.bind.bind(FP.bind),
      callbind   = bindbind(FP.call),
      applybind  = bindbind(FP.apply),
      bindapply  = applybind(FP.bind),
      _call      = callbind(FP.call),
      _apply     = callbind(FP.apply),
      _bind      = callbind(FP.bind),
      slice      = callbind(Array.prototype.slice),
      _hasOwn    = callbind(Object.prototype.hasOwnProperty);


  function isObject(o){
    if (o == null) return o !== null && o !== undefined;
    return typeof o === 'object' || typeof o === 'function';
  }

  var BROWSER = 'browser',
      NODE = 'node';


  var proxies = new WeakMap,
      farIDs = new WeakMap,
      nearIDs = new WeakMap,
      mirrors = Object.create(null);

  if (typeof global === 'undefined') {
    var items = [window],
        context = BROWSER,
        pipe = appjs,
        nid = 'bid',
        bid = 'nid',
        cross = 'node';
    nearIDs.set(window, 0);
  } else {
    var items = [global],
        context = NODE,
        pipe = window,
        nid = 'nid',
        bid = 'bid',
        cross = 'window';
    nearIDs.set(global, 0);
  }

  setTimeout(function(){
    var o = {}; o[bid] = 0;
    window[cross] = unwrap(o);
  }, 1000);

  function FarJSON(id){
    this[bid] = id;
    this.json = {};
    this.json[bid] = id;
    this.type = pipe.send('mirror-type', this).type;
  }

  FarJSON.prototype = {
    constructor: FarJSON,
    toJSON: function toJSON(){
      return this.json;
    },
    keyJSON: function keyJSON(key, value, desc){
      var o = {};
      o[bid] = this[bid];
      o.key = key;
      if (2 in arguments) {
        o.desc = desc;
      } else if (1 in arguments) {
        o.val = value;
      }
      return o;
    },
    argJSON: function argJSON(args, receiver){
      var o = {};
      o[bid] = this[bid];
      o.args = args;
      if (1 in arguments) {
        o.receiver = receiver;
      }
      return o;
    }
  };

  function decodePrimitive(o){
    switch (o.__val__) {
      case '__UND__': return undefined;
      case '__NaN__': return NaN;
      case '__Infinity__': return Infinity;
      case '__-Infinity__': return -Infinity;
      case '__-0__': return -0;
      default: return o.__val__;
    }
  }

  function encodePrimitive(o){
    switch (o) {
      case undefined:  return { __val__: '__UND__' };
      case Infinity:   return { __val__: '__Infinity__' };
      case -Infinity:  return { __val__: '__-Infinity__' };
      case NaN:        return { __val__: '__NaN__' };
      case 0:          return { __val__: 1 / o === -Infinity ? '__-0__' : 0 };
      default:         return { __val__: o === o ? o : '__NaN__' };
    }
  }

  function unwrap(obj){
    if (!isObject(obj)) {
      return obj;
    } else if (_hasOwn(obj, '__val__')) {
      return decodePrimitive(obj);
    } else if (_hasOwn(obj, bid)) {
      if (obj[bid] in mirrors) {
        return farIDs.get(mirrors[obj[bid]]);
      } else {
        var json = new FarJSON(obj[bid]);
        var proxy = new Mirror(json);
        proxies.set(proxy, json);
        farIDs.set(json, proxy);
        mirrors[json[bid]] = json;
        return proxy;
      }
    } else if (_hasOwn(obj, nid)) {
      return items[obj[nid]];
    } else {
      return obj;
    }
  }

  function wrap(obj){
    if (!isObject(obj)) {
      return encodePrimitive(obj);
    } else if (proxies.has(obj)) {
      return proxies.get(obj);
    } else {
      var id = nearIDs.get(obj);
      if (id === undefined) {
        nearIDs.set(obj, id = items.length);
        items.push(obj);
      }
      var o = {};
      o[nid] = id;
      return o;
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
      var i=0, k=[];
      for (k[i++] in obj);
      return k;
    },
    properties: function properties(obj, o){
      return Object.getOwnPropertyNames(obj);
    },
    get: function get(obj, o){
      return wrap(obj[o.key]);
    },
    proto: function proto(obj, o){
      return wrap(obj.__proto__);
    },
    set: function set(obj, o){
      obj[o.key] = unwrap(o.val);
      return wrap(true);
    },
    has: function has(obj, o){
      return wrap(o.key in obj);
    },
    hasOwn: function hasOwn(obj, o){
      return wrap(_hasOwn(obj, o.key));
    },
    delete: function delete_(obj, o){
      delete obj[o.key];
      return wrap(true);
    },
    define: function define(obj, o){
      if (_hasOwn(o.desc, 'value')) {
        o.desc.value = unwrap(o.desc.value);
      } else {
        o.desc.get = unwrap(o.desc.get);
        o.desc.set = unwrap(o.desc.set);
      }
      Object.defineProperty(obj, o.key, o.desc);
      return wrap(true);
    },
    describe: function describe(obj, o){
      var desc = Object.getOwnPropertyDescriptor(obj, o.key);
      if (desc) {
        desc.configurable = true;
        if (_hasOwn(desc, 'value')) {
          if (desc.value === undefined) desc.value = obj[o.key];
          desc.value = wrap(desc.value);
        } else {
          desc.get = wrap(desc.get);
          desc.set = wrap(desc.set);
        }
        return desc;
      } else if (desc === undefined && _hasOwn(obj, o.key)) {
        return { configurable: true,
                 enumerable: true,
                 writable: true,
                 value: wrap(obj[o.key]) };
      } else {
        return wrap(desc);
      }
    },
    apply: function apply(obj, o){
      try {
        return wrap(_apply(obj, unwrap(o.receiver), o.args.map(unwrap)));
      } catch (e) {
        return wrap(e);
      }
    },
    construct: function construct(obj, o){
      try {
        return wrap(new (bindapply(obj, [null].concat(o.args.map(unwrap)))));
      } catch (e) {
        return wrap(e);
      }
    },
  };

  function Mirror(json){
    var self = this;
    this.json = json;
    if (json.type === 'function') {
      return Proxy.createFunction(this,
        function(){ return self.apply(this, slice(arguments)) },
        function(){ return self.construct(slice(arguments)) }
      );
    } else {
      return Proxy.create(this, unwrap(pipe.send('mirror-proto', this.json)));
    }
  }


  Mirror.prototype = {
    keys: function keys(){
      return pipe.send('mirror-keys', this.json);
    },
    enumerate: function enumerate(){
      return pipe.send('mirror-enumerate', this.json);
    },
    getOwnPropertyNames: function getOwnPropertyNames(){
      return pipe.send('mirror-properties', this.json);
    },
    get: function get(rcvr, key){
      return unwrap(pipe.send('mirror-get', this.json.keyJSON(key)));
    },
    set: function set(rcvr, key, value){
      pipe.send('mirror-set', this.json.keyJSON(key, wrap(value)));
      return true;
    },
    getOwnPropertyDescriptor: function getOwnPropertyDescriptor(key){
      var desc = pipe.send('mirror-describe', this.json.keyJSON(key));
      if (desc) {
        desc.configurable = true;
        if (isObject(desc.value)) {
          desc.value = unwrap(desc.value);
        } else {
          desc.get = unwrap(desc.get);
          desc.set = unwrap(desc.set);
        }
      }
      return desc;
    },
    has: function has(key){
      return pipe.send('mirror-has', this.json.keyJSON(key));
    },
    hasOwn: function hasOwn(key){
      return pipe.send('mirror-hasOwn', this.json.keyJSON(key));
    },
    delete: function delete_(key){
      pipe.send('mirror-delete', this.json.keyJSON(key));
      return true;
    },
    defineProperty: function defineProperty(key, desc){
      desc = wrap(desc);
      if (_hasOwn(desc, 'value')) {
        desc.value = wrap(desc.value);
      } else {
        desc.get = wrap(desc.get);
        desc.set = wrap(desc.set);
      }
      pipe.send('mirror-define', this.json.keyJSON(key, null, desc));
    },
    apply: function apply(receiver, args){
      return unwrap(pipe.send('mirror-apply', this.json.argJSON(args.map(wrap), wrap(receiver))));
    },
    construct: function construct(args){
      return unwrap(pipe.send('mirror-construct', this.json.argJSON(args.map(wrap))));
    }
  };

  Object.keys(mirror).forEach(function(key){
    pipe.on('mirror-'+key, function(val, res){
      res.result = mirror[key](unwrap(val), val);
    });
  });
}

var app = module.exports = require('appjs');

app.get('/', function(req,res,next){
  res.send("Hello World!");
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
  this.show();
  this.openDevTools();
});

window.on("ready",function(){
  initPipe(this);
  this.runInBrowser(initPipe);
});

window.on("close",function(){
});
