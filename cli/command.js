var fs    = require('fs')
  , path  = require('path')
  , mkdir = require('mkdirp')
  , util  = require('./util')
  , config = require('./config')
  , exec = require('child_process').exec;

var init = exports.init = function(){
  
  if( arguments.length > 1 ) {
    util.abort(config.ERROR.tooMuchArguments);
  }

  var project_dir = process.cwd();

  if( arguments.length == 1 ) {
    project_dir = fs.realpathSync(arguments[0]);
  }

  // TODO create boilerplate

  // TODO create launcher

  util.log('OK',util.log.success);

}

var package = exports.package = function(){

}

var help = exports.help = function(){
  console.log(config.HELP);
}