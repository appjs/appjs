var fs = require('fs');
var path = require('path');
var util = require('./util');
var mkdir = require('mkdirp');
var config = require('./config');
var exec = require('child_process').exec;

var Init = exports.init = function(){

    if( arguments.length > 1 ) {
        util.abort(config.ERROR.tooMuchArguments);
    }

    var input_dir = process.cwd();

    if( arguments.length == 1 ) {
        input_dir = fs.realpathSync(arguments[0]);
    }

    util.log('generating sample application...');

    // TODO generate sample application!!

    util.log('OK',util.log.success);
}

var Package = exports.package = function(){

    if( arguments.length > 2 ) {
        util.abort(config.ERROR.tooMuchArguments);
    }

    var output_dir, input_dir  = process.cwd();

    if( arguments.length ) {
        input_dir = fs.realpathSync(arguments[0]);
    }

    if( arguments.length == 2 ) {
        output_dir = path.resolve(arguments[1]);

        if( output_dir == input_dir ){
            output_dir = path.resolve(input_dir,'build');
        }
    }

    mkdir.sync(output_dir);
    
    //TODO create installers etc.

    util.log('Done.','success');
}

var Help = exports.help = function(){
  console.log(config.HELP);
}