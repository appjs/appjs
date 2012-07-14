function initPipe(win){
  function isObject(o){
    if (o == null) return o !== null && o !== undefined;
    return typeof o === 'object' || typeof o === 'function';
  }

  var _call      = Function.prototype.call,
      _apply     = Function.prototype.apply,
      _bind      = Function.prototype.bind,
      _slice     = Array.prototype.slice,
      _hasOwn    = Object.prototype.hasOwnProperty;


  var proxies = new WeakMap,
      farIDs = new WeakMap,
      nearIDs = new WeakMap,
      mirrors = Object.create(null);

  if (!arguments.length) {
    var items = [window],
        pipe = appjs,
        nid = 'bid',
        bid = 'nid';
    nearIDs.set(window, 0);
  } else {
    var items = [global],
        pipe = win,
        nid = 'nid',
        bid = 'bid';
    nearIDs.set(global, 0);
  }

  setTimeout(function(){
    var o = {}; o[bid] = 0;
    pipe.context = unwrap(o);
  }, 100);

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

  var TRUE = { ø: true },
      FALSE = { ø: false },
      UNDEFINED = { ø: '_Δ' },
      NULL = { ø: null },
      NAN = { ø: '_ƒ' },
      INFINITY = { ø: '_Ω' },
      NEGINFINITY = { ø: '_-Ω' },
      NEGZERO = { ø: '_π' },
      ZERO = { ø: 0 };

  function encodePrimitive(o){
    switch (typeof o) {
      case 'boolean':      return o ? TRUE : FALSE;
      case 'undefined':    return UNDEFINED;
      case 'object':       return NULL;
      case 'string':       return { ø: o };
      case 'number':
        switch (o) {
          case Infinity:   return INFINITY;
          case -Infinity:  return NEGINFINITY;
          case NaN:        return NAN;
          case 0:          return 1 / o === -Infinity ? NEGZERO : ZERO;
          default:         return o !== o ? NAN : { ø: o };
        }
    }
  }

  function decodePrimitive(o){
    switch (o.ø) {
      case '_Δ': return undefined;
      case '_ƒ': return NaN;
      case '_Ω': return Infinity;
      case '_-Ω': return -Infinity;
      case '_π': return -0;
      default: return o.ø;
    }
  }

  function unwrap(obj){
    if (!isObject(obj)) {
      return obj;
    } else if (_hasOwn.call(obj, 'ø')) {
      return decodePrimitive(obj);
    } else if (_hasOwn.call(obj, bid)) {
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
    } else if (_hasOwn.call(obj, nid)) {
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
      return TRUE;
    },
    has: function has(obj, o){
      return wrap(o.key in obj);
    },
    hasOwn: function hasOwn(obj, o){
      return wrap(_hasOwn.call(obj, o.key));
    },
    delete: function delete_(obj, o){
      delete obj[o.key];
      return TRUE;
    },
    define: function define(obj, o){
      if (_hasOwn.call(o.desc, 'value')) {
        o.desc.value = unwrap(o.desc.value);
      } else {
        o.desc.get = unwrap(o.desc.get);
        o.desc.set = unwrap(o.desc.set);
      }
      Object.defineProperty(obj, o.key, o.desc);
      return TRUE;
    },
    describe: function describe(obj, o){
      var desc = Object.getOwnPropertyDescriptor(obj, o.key);
      if (desc) {
        desc.configurable = true;
        if (_hasOwn.call(desc, 'value')) {
          if (desc.value === undefined) desc.value = obj[o.key];
          desc.value = wrap(desc.value);
        } else {
          desc.get = wrap(desc.get);
          desc.set = wrap(desc.set);
        }
        return desc;
      } else if (desc === undefined && _hasOwn.call(obj, o.key)) {
        return { configurable: true,
                 enumerable: true,
                 writable: true,
                 value: wrap(obj[o.key]) };
      } else {
        return UNDEFINED;
      }
    },
    apply: function apply(obj, o){
      try {
        return wrap(_apply.call(obj, unwrap(o.receiver), o.args.map(unwrap)));
      } catch (e) {
        return wrap(e);
      }
    },
    construct: function construct(obj, o){
      try {
        return wrap(new (_bind.apply(obj, [null].concat(o.args.map(unwrap)))));
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
        function(){ return self.apply(this, _slice.call(arguments)) },
        function(){ return self.construct(_slice.call(arguments)) }
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
      if (_hasOwn.call(desc, 'value')) {
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
