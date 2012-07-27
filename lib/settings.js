module.exports = function createSettings(o){
  return new Settings(o);
};

module.exports.Settings = Settings;


function Settings(o){
  for (var k in o) {
    this[k] = o[k];
  }
}

var LOGSEVERITY = Settings.LOGSEVERITY = {
  VERBOSE      : -1,
  INFO         :  0,
  WARNING      :  1,
  ERROR        :  2,
  ERROR_REPORT :  3,
  DISABLE      : 99
};

var DEFAULT = '';

Settings.prototype = {
  JSFlags                : ' --harmony_proxies --harmony_collections --harmony_scoping',
  LogLevel               : LOGSEVERITY.DISABLE,
  PakLoading             : true,

  LocalesPakPath         : DEFAULT,
  ChromePakPath          : DEFAULT,
  LogFilePath            : DEFAULT,
  CachePath              : DEFAULT,
  UserStylesheetPath     : DEFAULT,

  UserAgent              : DEFAULT,
  ProductVersion         : DEFAULT,
  Locale                 : DEFAULT,

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
  WebSecurity            : true,
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
};
