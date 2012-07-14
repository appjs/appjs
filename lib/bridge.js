module.exports = function bridge(win){
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
    setTimeout(function(){ window.node = unwrap([bid, 0]) }, 100);
  } else {
    var items = [global],
        pipe = win,
        nid = 'nid',
        bid = 'bid';
    nearIDs.set(global, 0);
    setTimeout(function(){ win.window = unwrap([bid, 0]) }, 100);
  }



  var TRUE        = [ 'ø', true ],
      FALSE       = [ 'ø', false ],
      UNDEFINED   = [ 'ø', '_Δ' ],
      NULL        = [ 'ø', null ],
      NAN         = [ 'ø', '_ƒ' ],
      INFINITY    = [ 'ø', '_Ω' ],
      NEGINFINITY = [ 'ø', '_-Ω' ],
      NEGZERO     = [ 'ø', '_π' ],
      ZERO        = [ 'ø', 0 ];

  function encodePrimitive(o){
    switch (typeof o) {
      case 'boolean':      return o ? TRUE : FALSE;
      case 'undefined':    return UNDEFINED;
      case 'object':       return NULL;
      case 'string':       return ['ø', o];
      case 'number':
        switch (o) {
          case Infinity:   return INFINITY;
          case -Infinity:  return NEGINFINITY;
          case NaN:        return NAN;
          case 0:          return 1 / o === -Infinity ? NEGZERO : ZERO;
          default:         return o !== o ? NAN : ['ø', o];
        }
    }
  }

  function decodePrimitive(o){
    switch (o[1]) {
      case '_Δ': return undefined;
      case '_ƒ': return NaN;
      case '_Ω': return Infinity;
      case '_-Ω': return -Infinity;
      case '_π': return -0;
      default: return o[1];
    }
  }

  function unwrap(obj){
    if (!isObject(obj)) {
      return obj;
    } else if (obj[0] == 'ø') {
      return decodePrimitive(obj);
    } else if (obj[0] === bid) {
      if (obj[1] in mirrors) {
        return farIDs.get(mirrors[obj[1]]);
      } else {
        var proxy = new Mirror(obj);
        proxies.set(proxy, obj);
        farIDs.set(obj, proxy);
        mirrors[obj[1]] = obj;
        return proxy;
      }
    } else if (obj[0] === nid) {
      return items[obj[1]];
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
      return [nid, id];
    }
  }

  var mirror = {
    type: function type(obj){
      return [typeof obj];
    },
    keys: function keys(obj){
      return Object.keys(obj);
    },
    enumerate: function enumerate(obj){
      var i=0, k=[];
      for (k[i++] in obj);
      return k;
    },
    props: function props(obj){
      return Object.getOwnPropertyNames(obj);
    },
    get: function get(obj, key){
      return wrap(obj[key]);
    },
    proto: function proto(obj){
      return wrap(obj.__proto__);
    },
    has: function has(obj, key){
      return wrap(key in obj);
    },
    hasOwn: function hasOwn(obj, key){
      return wrap(_hasOwn.call(obj, key));
    },
    delete: function delete_(obj, key){
      delete obj[key];
      return TRUE;
    },
    define: function define(obj, key, desc){
      if (_hasOwn.call(desc, 'value')) {
        desc.value = unwrap(desc.value);
      } else {
        desc.get = unwrap(desc.get);
        desc.set = unwrap(desc.set);
      }
      Object.defineProperty(obj, key, desc);
      return TRUE;
    },
    describe: function describe(obj, key){
      var desc = Object.getOwnPropertyDescriptor(obj, key);
      if (desc) {
        desc.configurable = true;
        if (_hasOwn.call(desc, 'value')) {
          if (desc.value === undefined) desc.value = obj[key];
          desc.value = wrap(desc.value);
        } else {
          desc.get = wrap(desc.get);
          desc.set = wrap(desc.set);
        }
        return desc;
      } else if (desc === undefined && _hasOwn.call(obj, key)) {
        return { configurable: true,
                 enumerable: true,
                 writable: true,
                 value: wrap(obj[key]) };
      } else {
        return UNDEFINED;
      }
    },
    construct: function construct(obj, args){
      try {
        return wrap(new (_bind.apply(obj, [null].concat(args.map(unwrap)))));
      } catch (e) {
        return wrap(e);
      }
    },
  };

  if (win) {
    mirror.apply = function apply(obj, receiver, args){
      try {
        return wrap(_apply.call(obj, unwrap(receiver), args.map(unwrap)));
      } catch (e) {
        return wrap(e);
      }
    };
    mirror.set = function set(obj, key, value){
      obj[key] = unwrap(value);
      return TRUE;
    };
  } else {
    var proxyForProxy = function proxyForProxy(fn){
      return function(){ return _apply.call(fn, this, arguments) };
    }
    mirror.apply = function apply(obj, receiver, args){
      try {
        // workaround for bug where proxy functions are not valid DOM event handlers
        args = args.map(unwrap);
        if (obj.name === 'addEventListener' && typeof args[1] === 'function') {
          args[1] = proxyForProxy(args[1]);
        }
        return wrap(_apply.call(obj, unwrap(receiver), args));
      } catch (e) {
        return wrap(e);
      }
    };
    mirror.set = function set(obj, key, value){
      value = unwrap(value);
      obj[key] = value;
      if (obj[key] === null && typeof value === 'function') {
        // workaround for bug where proxy functions are not valid DOM event handlers
        obj[key] = proxyForProxy(value);
      }
      return TRUE;
    };
  }



  function Mirror(obj){
    var self = this;
    var type = pipe.send('Δtype', obj)[0];
    this.a = obj;
    this.b = obj.concat(null);
    this.c = obj.concat(null, null);
    if (type === 'function') {
      return Proxy.createFunction(this,
        function(){ return self.apply(this, _slice.call(arguments)) },
        function(){ return self.construct(_slice.call(arguments)) }
      );
    } else {
      return Proxy.create(this, unwrap(pipe.send('Δproto', this.a)));
    }
  }


  Mirror.prototype = {
    keys: function keys(){
      return pipe.send('Δkeys', this.a);
    },
    enumerate: function enumerate(){
      return pipe.send('Δenumerate', this.a);
    },
    getOwnPropertyNames: function getOwnPropertyNames(){
      return pipe.send('Δprops', this.a);
    },
    get: function get(rcvr, key){
      this.b[2] = key;
      if (wrap(rcvr) !== this.a) {
        console.log(rcvr);
      }
      return unwrap(pipe.send('Δget', this.b));
    },
    set: function set(rcvr, key, value){
      this.c[2] = key;
      this.c[3] = wrap(value);
      if (wrap(rcvr) !== this.a) {
        console.log(rcvr);
      }
      pipe.send('Δset', this.c);
      return true;
    },
    getOwnPropertyDescriptor: function getOwnPropertyDescriptor(key){
      this.b[2] = key;
      var desc = pipe.send('Δdescribe', this.b);
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
      this.b[2] = key;
      return pipe.send('Δhas', this.b);
    },
    hasOwn: function hasOwn(key){
      this.b[2] = key;
      return pipe.send('ΔhasOwn', this.b);
    },
    delete: function delete_(key){
      this.b[2] = key;
      pipe.send('Δdelete', this.b);
      return true;
    },
    defineProperty: function defineProperty(key, desc){
      this.c[2] = key;
      this.c[3] = desc = wrap(desc);
      if (_hasOwn.call(desc, 'value')) {
        desc.value = wrap(desc.value);
      } else {
        desc.get = wrap(desc.get);
        desc.set = wrap(desc.set);
      }
      pipe.send('Δdefine', this.c);
    },
    apply: function apply(receiver, args){
      this.c[2] = wrap(receiver);
      this.c[3] = args.map(wrap);
      return unwrap(pipe.send('Δapply', this.c));
    },
    construct: function construct(args){
      this.b[2] = args.map(wrap);
      return unwrap(pipe.send('Δconstruct', this.b));
    }
  };

  Object.keys(mirror).forEach(function(key){
    pipe.on('Δ'+key, function(val, res){
      res.result = mirror[key](unwrap(val), val[2], val[3]);
    });
  });
}
