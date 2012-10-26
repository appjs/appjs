Path    = require("path");
Log     = require("just-logger").Logger;


/**
 *  Constructor method.
 *  file_path can be null. It uses process.stdout by default
 */
function Logger(file_path){
  this._logger = new Log(file_path);
  return this;
}

Logger.LEVELS = Log.levels;


/**
 * Creates an empty
 */
Logger.__defineGetter__("empty", function(){
  var log = {}
  for ( var i = 0, l = Logger.LEVELS.length; i < l; i++ ){
    log[ Logger.LEVELS[i] ] = function(){};
  }
  log.setFile = function(){};

  return log;
});


for ( var i = 0, l = Logger.LEVELS.length; i < l; i++ ){
  (function(lev){
    Logger.prototype[ lev ] = function(){
      return this._logger[ lev ].apply(this._logger, arguments);
    };
  })(Logger.LEVELS[i]);
}


/**
 * Returns the 'loglevels'
 */
Logger.prototype.__defineGetter__("levels", function(){
  return Logger.LEVELS;
});


/**
 *  Setter method that allows you to set
 *  the format of the output.
 *  the callback accepts (level, date, message)
 */
Logger.prototype.__defineSetter__("format", function(fn){
  this._logger.format = fn;
});



/**
 *  Set the level of the log.
 *  Accepts Number or String (see Logger.LEVELS)
 */
Logger.prototype.__defineSetter__("level", function(l){
  if ( typeof l == 'number' && Logger.LEVELS[ l ] ){
    l = Logger.LEVELS[ l ];
  }
  return this._logger.setLevel( l );
});
Logger.prototype.__defineGetter__("level", function(){
  return Logger.LEVELS[ this._logger.log_level_index ];
});


/**
 *  Set the path/to/file of the file used for writing log
 */
Logger.prototype.setFile = function(file_path){
  this._logger.useFile( file_path );
};


module.exports = Logger;