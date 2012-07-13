module.exports = function(){
  function Bitfield(values, source, extended){
    return Proxy.create(new BitfieldHandler(values, source, extended), Bitfield.prototype);
  }

  Bitfield.prototype.toJSON = function toJSON(){
    var out = {};
    Object.keys(this).forEach(function(key){
      out[key] = this[key];
    }, this);
    return out;
  }

  function BitfieldHandler(values, source, extended){
    this.values = values;
    this.source = source;
    this.extended = extended;
    this.list = Object.keys(values).sort();
    return
  }

  function list(){
    return this.list;
  }

  function has(key){
    return this.values.hasOwnProperty(key);
  }

  function get(key){
    return (this.values[key] & style.call(this.source, this.extended)) > 0;
  }

  function set(key, value){
    var val = style.call(this.source, this.extended);
    value ? (val |= this.values[key]) : (val &= ~this.values[key]);
    style.call(this.source, val, this.extended);
  }

  BitfieldHandler.prototype = {
    getOwnPropertyNames: list,
    keys: list,
    enumerate: function(){
      var i=0, k=[];
      for (k[i++] in Bitfield.prototype);
      return this.list.concat(k);
    },
    has: function(key){
      return has.call(this, key) || key in Bitfield.prototype
    },
    hasOwn: has,
    get: function(rcvr, key){
      return this.values.hasOwnProperty(key) ? get.call(this, key) : Bitfield.prototype[key];
    },
    set: function(rcvr, key, val){
      this.values.hasOwnProperty(key) ? set.call(this, key, val) : (Bitfield.prototype[key] = val)
    },
    delete: function(key){
      set.call(this, key, false);
    },
    getOwnPropertyDescriptor: function(key){
      return { enumerable: true,
               configurable: true,
               writable: true,
               value: get.call(this, key) }
    },
    defineProperty: function(key, desc){
      if ('value' in desc) {
        set.call(this, key, desc.value);
      } else {
        return false;
      }
    }
  };

  Bitfield.styles = {
    overlapped       : 0x00000000,
    maximizeBox      : 0x00010000,
    minimizeBox      : 0x00020000,
    sizeBox          : 0x00040000,
    sysMenu          : 0x00080000,
    hScroll          : 0x00100000,
    vScroll          : 0x00200000,
    dlgFrame         : 0x00400000,
    border           : 0x00800000,
    caption          : 0x00C00000,
    maximize         : 0x01000000,
    clipChildren     : 0x02000000,
    clipSiblings     : 0x04000000,
    disabled         : 0x08000000,
    visible          : 0x10000000,
    minimize         : 0x20000000,
    child            : 0x40000000,
    popup            : 0x80000000,
    overlappedWindow : 0x00CF0000,
    popupWindow      : 0x80880000
  };

  Bitfield.exStyles = {
    left             : 0x00000000,
    dlgModalFrame    : 0x00000001,
    noParentNotify   : 0x00000004,
    topMost          : 0x00000008,
    acceptFiles      : 0x00000010,
    transparent      : 0x00000020,
    mdiChild         : 0x00000040,
    toolWindow       : 0x00000080,
    windowEdge       : 0x00000100,
    clientEdge       : 0x00000200,
    overlappedWindow : 0x00000300,
    contextHelp      : 0x00000400,
    rightScrollbar   : 0x00001000,
    rtlReading       : 0x00002000,
    controlParent    : 0x00010000,
    staticEdge       : 0x00020000,
    appWindow        : 0x00040000,
    layered          : 0x00080000,
    noInheritLayout  : 0x00100000,
    layoutRtl        : 0x00400000,
    composited       : 0x02000000,
    noActivate       : 0x08000000,
    paletteWindow    : 0x00000188
  };

  var style;

  return function(Window){
    style = Window.prototype.style;
    delete Window.prototype.style;
    return function(window){
      if (typeof Proxy === 'undefined') return;
      window.styles = new Bitfield(Bitfield.styles, window, false);
      window.exStyles = new Bitfield(Bitfield.exStyles, window, true);
    };
  };
}();
