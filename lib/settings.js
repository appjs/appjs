var fs           = require('fs'),
    path         = require('path'),
    decorate     = require('./utils').decorate,
    constants    = require('./utils').constants,
    isObject     = require('./utils').isObject,
    platformPath = require('./bindings').platformPath;


module.exports = {
  AppSettings: AppSettings,
  WindowSettings: WindowSettings,
  IconGroup: IconGroup
};

function AppSettings(o){
  for (var k in Object(o)) {
    this[k] = o[k];
  }
}

AppSettings.create = function createAppSettings(o){
  return new AppSettings(o);
};

constants(AppSettings, {
  LOGLEVEL_VERBOSE     : -1,
  LOGLEVEL_INFO        :  0,
  LOGLEVEL_WARNING     :  1,
  LOGLEVEL_ERROR       :  2,
  LOGLEVEL_ERROR_REPORT:  3,
  LOGLEVEL_DISABLED    : 99
});


decorate(AppSettings.prototype, {
  JSFlags                : ' --harmony_proxies --harmony_collections --harmony_scoping',
  LogLevel               : AppSettings.LOGLEVEL_DISABLED,
  PakLoading             : true,

  LocalesPakPath         : path.resolve(platformPath, 'data', 'locales'),
  ChromePakPath          : path.resolve(platformPath, 'data', 'chrome.pak'),
  LogFilePath            : '',
  CachePath              : '',
  UserStylesheetPath     : '',

  UserAgent              : '',
  ProductVersion         : '',
  Locale                 : '',

  StandardFont           : 'Times New Roman',
  FixedFont              : 'Courier New',
  SerifFont              : 'Times New Roman',
  SansSerifFont          : 'Arial',
  CursiveFont            : 'Script',
  FantasyFont            : 'Impact',
  DefaultEncoding        : 'ISO-8859-1',

  DefaultFontSize        : 16,
  FixedFontSize          : 13,
  MinFontSize            : 0,
  MinLogicalFontSize     : 6,

  SiteSpecificQuirks     : false,
  CaretBrowsing          : false,
  UserStylesheets        : false,
  AcceleratedCompositing : false,

  XSSAuditer             : true,
  HyperlinkAuditing      : true,
  Plugins                : true,
  Java                   : true,
  PageCache              : true,
  ImageLoading           : true,
  EncodingDetector       : true,
  RemoteFonts            : true,
  DevTools               : true,
  TabToLinks             : true,
  ResizingTextareas      : true,
  FitStandaloneImages    : true,

  Accelerated2dCanvas    : true,
  AcceleratedFilters     : true,
  AcceleratedLayers      : true,
  AcceleratedPainting    : true,
  AcceleratedPlugins     : true,
  AcceleratedVideo       : true,

  JSCanRun               : true,
  JSCanAccessClipboard   : true,
  JSCanCloseWindows      : true,
  JSCanOpenWindows       : true,

  FileAccessFromFileURLs : true,
  UniversalFileURLAccess : true,
  UserStyles             : true,
  HistoryAPI             : true,
  AppCacheAPI            : true,
  DatabaseAPI            : true,
  DOMPasteAPI            : true,
  DragAndDropAPI         : true,
  LoadDrops              : true,
  FullscreenAPI          : true,
  LocalStorageAPI        : true,
  WebGLAPI               : true,
});



function WindowSettings(o){
  if (typeof o === 'string') {
    if (isObject(arguments[1])) {
      arguments[1].url = o;
      o = arguments[1];
    } else {
      o = { url: o };
    }
  }
  for (var k in Object(o)) {
    this[k] = o[k];
  }
  this.icons = new IconGroup(this.icons);
  if (!this.name || typeof this.name !== 'string') {
    this.name = WindowSettings.createName();
  }
}

WindowSettings.create = function createWindowSettings(o){
  return new WindowSettings(o);
};

WindowSettings.createName = function(){
  var names = [
    'alpha', 'bravo', 'charlie', 'delta', 'echo', 'foxtrot', 'golf',
    'hotel', 'india', 'juliet', 'kilo', 'lima', 'mike', 'november',
    'oscar', 'papa', 'quebec', 'romeo', 'sierra', 'tango', 'uniform',
    'victor', 'whiskey', 'x-ray', 'yankee', 'zulu'
  ];
  var index = 0, suffix = '';

  return function createName(){
    var name = names[index++] + suffix;
    if (index === names.length) {
      index = 0;
      suffix = (suffix || 0) + 1;
    }
    return name;
  };
}();

decorate(WindowSettings.prototype, {
  url           :'http://appjs/',
  name          : null,
  icons         : null,
  width         : 800,
  height        : 600,
  left          : -1,
  top           : -1,
  opacity       : 1,
  alpha         : false,
  showChrome    : true,
  autoResize    : false,
  resizable     : true,
  showResizeGrip: false,
  fullscreen    : false,
  topmost       : false,
});

var namedSizes = ['smaller', 'small', 'big', 'bigger'],
    numberSizes = [16, 32, 64, 128];

var sizeMap = {
  16: 'smaller',
  32: 'small',
  64: 'big',
  128: 'bigger',
  smaller: 'smaller',
  small: 'small',
  big: 'big',
  bigger: 'bigger'
};


function IconGroup(o){
  if (o instanceof IconGroup) {
    Object.keys(o).forEach(function(key){
      this[key] = o[key];
    }, this);
  } else if (typeof o === 'string') {
    this.setRoot(o);
  } else if (isObject(o)) {
    this.setObject(o);
  } else if (Array.isArray(o)) {
    this.setArray(o);
  } else {
    namedSizes.forEach(function(key){
      this[key] = '';
    }, this);
  }
}

IconGroup.create = function createIconGroup(o){
  return new IconGroup(o);
};


decorate(IconGroup.prototype, [
  function setRoot(root){
    if (fs.existsSync(root + '/small.png')) {
      var sizes = namedSizes;
    } else if (fs.existsSync(root + '/16.png')) {
      var sizes = numberSizes;
    } else {
      return this;
    }
    sizes.forEach(function(size){
      this[sizeMap[size]] = path.resolve(root, size+'.png');
    }, this);
    this.resolve();
    return this;
  },
  function setObject(obj){
    for (var k in obj) {
      if (sizeMap.hasOwnProperty(k)) {
        this[sizeMap[k]] = obj[k];
      }
    }
    this.resolve();
    return this;
  },
  function setArray(array){
    array.forEach(function(item){
      if (typeof item === 'string' && /\.png$/.test(item)) {
        var name = path.basename(item).slice(0, -4);
        if (sizeMap.hasOwnProperty(name)) {
          this[sizeMap[name]] = item;
        }
      }
    }, this);
    this.resolve();
    return this;
  },
  function resolve(){
    Object.keys(this).forEach(function(key){
      this[key] = path.resolve(this[key]);
      if (!fs.existsSync(this[key])) {
        delete this[key];
      }
    }, this);
    return this;
  }
]);
