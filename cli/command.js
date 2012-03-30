/**
 * (The MIT License)
 *
 * Copyright (c) 2012 Morteza Milani <mrtz.milani@gmail.com> and other 
 * AppJS contributors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **/
 
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
    manifest.entry_point = './' + path.relative(input_dir, manifest.entry_point);
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

    var content = JSON.stringify(pack.generate(input_dir,manifest));

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
