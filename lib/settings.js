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

Settings.prototype = {
  JSFlags                : ' --harmony_proxies --harmony_collections --harmony_scoping',
  LocalesPakPath         : '',
  ChromePakPath          : '',
  LogFilePath            : '',
  CachePath              : '',
  UserAgent              : '',
  ProductVersion         : '',
  Locale                 : '',
  PakLoading             : true,
  LogLevel               : LOGSEVERITY.DISABLE,

  StandardFont           : 'Times New Roman',
  FixedFont              : 'Courier New',
  SerifFont              : 'Times New Roman',
  SansSerifFont          : 'Arial',
  CursiveFont            : 'Script',
  FantasyFont            : 'Impact',
  DefaultEncoding        : 'ISO-8859-1',
  UserStylesheetPath     : '',

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

  JS                     : true,
  JSClipboardAccess      : true,
  JSClosesWindow         : true,
  JSOpensWindows         : true,

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
