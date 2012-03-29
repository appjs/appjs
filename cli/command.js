var fs = require('fs');
var path = require('path');
var util = require('./util');
var pack = require('./pack');
var mkdir = require('mkdirp');
var config = require('./config');

var init = exports.init = function(){

    if( arguments.length > 1 ) {
        util.abort(config.ERROR.tooMuchArguments);
    }

    var input_dir = process.cwd();

    if( arguments.length == 1 ) {
        input_dir = fs.realpathSync(arguments[0]);
    }

    var manifest = config.manifest;

    manifest.compile = this.compile || manifest.compile;
    manifest.embed = this.embed || manifest.embed;
    manifest.entry_point = this.entry || manifest.entry_point;
    manifest.build_dir = this.out || manifest.build_dir;
    manifest.build_dir = path.relative(input_dir,manifest.build_dir);
    manifest.extra = this.extra || manifest.extra;

    util.log('generating manifest file...');
    //@TODO generate a human-readable output!
    input_dir = path.join(input_dir,'manifest.json');
    fs.writeFileSync(input_dir,JSON.stringify(config.manifest),'utf-8');
    util.log('OK',util.log.success);
}

var packageF = exports.package = function(){

    if( arguments.length > 2 ) {
        util.abort(config.ERROR.tooMuchArguments);
    }

    var input_dir  = process.cwd();

    if( arguments.length == 1 ) {
        input_dir = fs.realpathSync(arguments[0]);
    }

    if( arguments.length == 2 ) {
        input_dir  = fs.realpathSync(arguments[0]);
        output_dir = path.resolve(arguments[1]);

        if( output_dir == input_dir ){
            output_dir = path.resolve(input_dir,'build');
        }
    }

    util.log('Reading manifest file...');

    try {
        var manifest = JSON.parse(fs.readFileSync(path.join(input_dir,'manifest.json')));
    } catch (e) {
        util.abort('Manifest file not found.');
    }
    
    manifest.compile = this.compile || manifest.compile;
    manifest.embed = this.embed || manifest.embed;
    manifest.entry_point = this.entry || manifest.entry_point;
    //manifest.build_dir = output_dir || this.out || manifest.build_dir;
    manifest.extra = this.extra || manifest.extra;

    if( typeof output_dir == 'undefined'){
        output_dir = path.resolve(input_dir,manifest.build_dir);
    }

    util.log('Generating package...');

    var content = JSON.stringify(pack.generate(input_dir,manifest.entry_point,manifest.extra));

    var resource_path = path.resolve(output_dir,'resources.json');

    mkdir.sync(output_dir);
    fs.writeFileSync(resource_path,content);

    util.log('Done.','success');
}

var build = exports.build = function(){

    if( arguments.length > 2 ) {
        util.abort(config.ERROR.tooMuchArguments);
    }

    var input_dir  = process.cwd();

    if( arguments.length == 1 ) {
        input_dir = fs.realpathSync(arguments[0]);
    }

    if( arguments.length == 2 ) {
        input_dir  = fs.realpathSync(arguments[0]);
        output_dir = path.resolve(arguments[1]);

        if( output_dir == input_dir ){
            output_dir = path.resolve(input_dir,'build');
        }
    }

    util.log('Reading manifest file...');

    try {
        var manifest = JSON.parse(fs.readFileSync(path.join(input_dir,'manifest.json')));
    } catch (e) {
        util.abort('Manifest file not found.');
    }
    
    manifest.compile = this.compile || manifest.compile;
    manifest.embed = this.embed || manifest.embed;
    manifest.entry_point = this.entry || manifest.entry_point;
    //manifest.build_dir = output_dir || this.out || manifest.build_dir;
    manifest.extra = this.extra || manifest.extra;

    if( typeof output_dir == 'undefined'){
        output_dir = path.resolve(input_dir,manifest.build_dir);
    }

    util.log('Generating package...');

    var content = JSON.stringify(pack.generate(input_dir,manifest.entry_point,manifest.extra));

    var resource_path = path.resolve(output_dir,'resources.json');

    mkdir.sync(output_dir);
    fs.writeFileSync(resource_path,content);

    //@TODO check to see if node is compiled and binary file is out there.
    //@TODO copy the binary file to build dir.
    util.log('Done.','success');
}
/*
module.exports = {

    "init": function(){

        if( arguments.length > 1 ) {
            util.abort(config.ERROR.tooMuchArguments);
        }

        var input_dir = process.cwd();

        if( arguments.length == 1 ) {
            input_dir = fs.realpathSync(arguments[0]);
        }

        var manifest = config.manifest;

        manifest.compile = this.compile || manifest.compile;
        manifest.embed = this.embed || manifest.embed;
        manifest.entry_point = this.entry || manifest.entry_point;
        manifest.build_dir = this.out || manifest.build_dir;
        manifest.build_dir = path.relative(input_dir,manifest.build_dir);
        manifest.extra = this.extra || manifest.extra;

        util.log('generating manifest file...');
        //@TODO generate a human-readable output!
        input_dir = path.join(input_dir,'manifest.json');
        fs.writeFileSync(input_dir,JSON.stringify(config.manifest),'utf-8');
        util.log('OK',util.log.success);
    },
    "package": function(){

        if( arguments.length > 2 ) {
            util.abort(config.ERROR.tooMuchArguments);
        }

        var input_dir  = process.cwd();

        if( arguments.length == 1 ) {
            input_dir = fs.realpathSync(arguments[0]);
        }

        if( arguments.length == 2 ) {
            input_dir  = fs.realpathSync(arguments[0]);
            output_dir = path.resolve(arguments[1]);

            if( output_dir == input_dir ){
                output_dir = path.resolve(input_dir,'build');
            }
        }

        util.log('Reading manifest file...');

        try {
            var manifest = JSON.parse(fs.readFileSync(path.join(input_dir,'manifest.json')));
        } catch (e) {
            util.abort('Manifest file not found.');
        }
        
        manifest.compile = this.compile || manifest.compile;
        manifest.embed = this.embed || manifest.embed;
        manifest.entry_point = this.entry || manifest.entry_point;
        //manifest.build_dir = output_dir || this.out || manifest.build_dir;
        manifest.extra = this.extra || manifest.extra;

        if( typeof output_dir == 'undefined'){
            output_dir = path.resolve(input_dir,manifest.build_dir);
        }

        util.log('Generating package...');

        var content = JSON.stringify(pack.generate(input_dir,manifest.entry_point,manifest.extra));

        var resource_path = path.resolve(output_dir,'resources.json');

        mkdir.sync(output_dir);
        fs.writeFileSync(resource_path,content);

        util.log('Done.','success');
    },
    "build":function(){

    }

}*/