module.exports = function(){
  var _send = window.appjs.send;
  var events = {};

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

  var appjs = window.appjs = {
    on: function on(types, handler){
      types.split(/\s+/).forEach(function(type){
        var listeners = events[type] || (events[type] = []);
        if (!~listeners.indexOf(handler)) {
          listeners.push(handler);
        }
      });
    },
    off: function off(types, handler){
      types.split(/\s+/).forEach(function(type){
        if (events[type]) {
          var index = events[type].indexOf(handler);
          ~index && events[type].splice(index, 1);
        }
      });
    },
    once: function once(types, handler){
      appjs.on(types, function single(){
        appjs.off(types, single);
        return handler.apply(this, arguments);
      });
    },
    onmessage: function onmessage(msg){
      var res = {};
      msg = decode(msg);

      if (msg && msg.type && events[msg.type]) {
        var listeners = events[msg.type];
        for (var i=0; i < listeners.length; i++) {
          listeners[i].call(appjs, msg.msg, res);
        }
      }
      return encode(res.result === undefined ? {} : res.result);
    },
    send: function send(type, msg){
      msg = { type: type, msg: msg };
      return decode(_send.call(appjs, encode(msg)));
    },
    log: function log(obj){
      appjs.send('log', obj);
    }
  };

  appjs.on('log', function(obj){
    console.log(obj);
  });
}
