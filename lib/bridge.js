module.exports = function bridge(nativeWindow, win, unwrapEphemeral){
  function isObject(o){
    if (o == null) return o !== null && o !== undefined;
    return typeof o === 'object' || typeof o === 'function';
  }

  function encode(msg){
    if (msg == null)
      return '{}';
    else if (typeof msg === 'object')
      return JSON.stringify(msg);
    else if (typeof msg === 'string')
      return msg;
    else
      throw new TypeError("Tried to encode invalid type");
  }

  function decode(msg){
    try {
      return JSON.parse(msg);
    } catch (e) {
      return { error: e };
    }
  }


  var _call   = Function.prototype.call,
      _apply  = Function.prototype.apply,
      _bind   = Function.prototype.bind,
      _slice  = Array.prototype.slice,
      _hasOwn = Object.prototype.hasOwnProperty,
      char    = String.fromCharCode;

  var foreignObjects = new WeakMap,
      foreignIDs = new WeakMap,
      localObjects = new WeakMap,
      localIDs = new WeakMap,
      localCanonicals = [],
      foreignCanonicals = Object.create(null);

  var TOKEN       = char(1),
      LITERAL     = char(2),
      TRUE        = [TOKEN, 1],
      FALSE       = [TOKEN, 2],
      UNDEFINED   = [TOKEN, 3],
      NULL        = [TOKEN, 4],
      NAN         = [TOKEN, 5],
      INFINITY    = [TOKEN, 6],
      NEGINFINITY = [TOKEN, 7],
      NEGZERO     = [TOKEN, 8],
      ZERO        = [TOKEN, 9];

  var tokens = [, true, false, undefined, null, NaN, Infinity, -Infinity, -0, 0];

  var pipe = function(source, listeners){
    source.onmessage = function onmessage(msg){
      var res = {};
      msg = decode(msg);
      listeners[msg.type] && listeners[msg.type](msg.msg, res);
      return encode(res.result === undefined ? {} : res.result);
    };

    return {
      send: function send(type, msg){
        msg = { type: type, msg: msg };
        return decode(source.send(encode(msg)));
      },
      listen: function listen(type, handler){
        listeners[type] = handler;
      }
    };
  }(nativeWindow || appjs, {});

  if (!nativeWindow) {
    var LOCAL = char(15), FOREIGN = char(16);
    var logger = function logger(type, error){
      if (typeof node !== 'undefined') {
        logger = function logger(type, error){
          node.process.emit('appjs-log', type, 'browser', error);
        };
        logger(type, context, error);
      }
    }
  } else {
    var LOCAL = char(16), FOREIGN = char(15);
    var logger = function logger(type, error){
      process.emit('appjs-log', type, 'node', error);
    };
    nativeWindow.once('context-released', function released(){
      mirror = {
        type      : function(){ return ['object'] },
        keys      : function(){ return [] },
        enumerate : function(){ return [] },
        props     : function(){ return [] },
        get       : function(){ return UNDEFINED },
        set       : function(){ return TRUE },
        proto     : function(){ return NULL },
        has       : function(){ return FALSE },
        hasOwn    : function(){ return FALSE },
        delete    : function(){ return TRUE },
        define    : function(){ return TRUE },
        describe  : function(){ return UNDEFINED },
        construct : function(){ return UNDEFINED },
        apply     : function(){ return UNDEFINED }
      };
      unwrap = wrap = pipe.send = pipe.listen = function(){ return {} };
      nativeWindow.onmessage = function(){ return ''; }
      nativeWindow = null;
      foreignObjects = foreignIDs = foreignCanonicals = null;
      localObjects = localIDs = localCanonicals = null;
    });
  }



  wrap(nativeWindow ? global : window);

  function initForeign(obj, proxy){
    foreignObjects.set(proxy, obj);
    foreignIDs.set(obj, proxy);
    foreignCanonicals[obj[1]] = obj;
    return proxy;
  }

  function unwrap(obj){
    if (!isObject(obj)) {
      return obj;
    } else if (obj[0] === TOKEN) {
      return tokens[obj[1]];
    } else if (obj[0] === LITERAL) {
      return obj[1];
    } else if (obj[0] === FOREIGN) {
      if (obj[1] in foreignCanonicals) {
        return foreignIDs.get(foreignCanonicals[obj[1]]);
      } else {
        return initForeign(obj, new Mirror(obj));
      }
    } else if (obj[0] === LOCAL && localCanonicals[obj[1]]) {
      return localObjects.get(localCanonicals[obj[1]]);
    } else {
      return obj;
    }
  }

  function wrap(obj){
    if (!isObject(obj)) {
      switch (typeof obj) {
        case 'boolean':      return obj ? TRUE : FALSE;
        case 'undefined':    return UNDEFINED;
        case 'object':       return NULL;
        case 'string':       return [LITERAL, obj];
        case 'number':
          switch (obj) {
            case Infinity:   return INFINITY;
            case -Infinity:  return NEGINFINITY;
            case NaN:        return NAN;
            case 0:          return 1 / obj === -Infinity ? NEGZERO : ZERO;
            default:         return obj !== obj ? NAN : [LITERAL, obj];
          }
      }
    } else if (foreignObjects.has(obj)) {
      return foreignObjects.get(obj);
    } else if (localIDs.has(obj)) {
      return localIDs.get(obj);
    } else {
      var id = [LOCAL, localCanonicals.length];
      localObjects.set(id, obj);
      localIDs.set(obj, id);
      localCanonicals[id[1]] = id;
      return id;
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
      if ('value' in desc) {
        desc.value = unwrap(desc.value);
      } else {
        if (desc.get) desc.get = unwrap(desc.get);
        if (desc.set) desc.set = unwrap(desc.set);
      }
      Object.defineProperty(obj, key, desc);
      return TRUE;
    },
    describe: function describe(obj, key){
      var desc = Object.getOwnPropertyDescriptor(obj, key);
      if (desc) {
        desc.configurable = true;
        if ('value' in desc) {
          if (desc.value === undefined) desc.value = obj[key];
          desc.value = wrap(desc.value);
        } else {
          if (desc.get) desc.get = wrap(desc.get);
          if (desc.set) desc.set = wrap(desc.set);
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
        logger('Constructor throw', e);
        return wrap(e);
      }
    },
  };

  if (nativeWindow) {
    mirror.apply = function apply(obj, receiver, args){
      try {
        return wrap(_apply.call(obj, unwrap(receiver), args.map(unwrap)));
      } catch (e) {
        logger('Apply throw', e);
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
    };
    mirror.apply = function apply(obj, receiver, args){
      // workaround for bug where proxy functions are not valid DOM event handlers
      args = args.map(unwrap);
      if (obj.name === 'addEventListener' && typeof args[1] === 'function') {
        args[1] = proxyForProxy(args[1]);
      }
      try {
        return wrap(_apply.call(obj, unwrap(receiver), args));
      } catch (e) {
        logger('Apply throw', e);
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
    this.id = obj;
    this.id2 = obj.concat(null);
    this.id3 = obj.concat(null, null);
    if (type === 'function') {
      return Proxy.createFunction(this,
        function(){ return self.apply(this, _slice.call(arguments)) },
        function(){ return self.construct(_slice.call(arguments)) }
      );
    } else {
      return Proxy.create(this, unwrap(pipe.send('Δproto', this.id)));
    }
  }


  Mirror.prototype = {
    keys: function keys(){
      return pipe.send('Δkeys', this.id);
    },
    enumerate: function enumerate(){
      return pipe.send('Δenumerate', this.id);
    },
    getOwnPropertyNames: function getOwnPropertyNames(){
      return pipe.send('Δprops', this.id);
    },
    get: function get(rcvr, key){
      this.id2[2] = key;
      return unwrap(pipe.send('Δget', this.id2));
    },
    set: function set(rcvr, key, value){
      this.id3[2] = key;
      this.id3[3] = wrap(value);
      pipe.send('Δset', this.id3);
      return true;
    },
    getOwnPropertyDescriptor: function getOwnPropertyDescriptor(key){
      this.id2[2] = key;
      var desc = pipe.send('Δdescribe', this.id2);
      if (desc) {
        desc.configurable = true;
        if ('value' in desc) {
          desc.value = unwrap(desc.value);
        } else {
          if (desc.get) desc.get = unwrap(desc.get);
          if (desc.set) desc.set = unwrap(desc.set);
        }
      }
      return desc;
    },
    has: function has(key){
      this.id2[2] = key;
      return pipe.send('Δhas', this.id2);
    },
    hasOwn: function hasOwn(key){
      this.id2[2] = key;
      return pipe.send('ΔhasOwn', this.id2);
    },
    delete: function delete_(key){
      this.id2[2] = key;
      pipe.send('Δdelete', this.id2);
      return true;
    },
    defineProperty: function defineProperty(key, desc){
      this.id3[2] = key;
      this.id3[3] = desc;
      if ('value' in desc) {
        desc.value = wrap(desc.value);
      } else {
        if (desc.get) desc.get = wrap(desc.get);
        if (desc.set) desc.set = wrap(desc.set);
      }
      pipe.send('Δdefine', this.id3);
      return true;
    },
    apply: function apply(receiver, args){
      this.id3[2] = wrap(receiver);
      this.id3[3] = args.map(wrap);
      return unwrap(pipe.send('Δapply', this.id3));
    },
    construct: function construct(args){
      this.id2[2] = args.map(wrap);
      return unwrap(pipe.send('Δconstruct', this.id2));
    }
  };


  Object.keys(mirror).forEach(function(key){
    pipe.listen('Δ'+key, function(val, res){
      res.result = mirror[key](unwrap(val), val[2], val[3]);
    });
  });

  if (nativeWindow) {
    void function(){
      var id = [FOREIGN, 0];
      unwrapEphemeral(win).morph(Mirror.prototype, {
        id:  id,
        id2: [FOREIGN, 0, null],
        id3: [FOREIGN, 0, null, null]
      });
      initForeign(id, win);
    }();
  } else {
    window.node = unwrap([FOREIGN, 0]);
  }
}
