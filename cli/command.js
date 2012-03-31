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
var exec = require('child_process').exec;

var init = exports.init = function(){

    if( arguments.length > 1 ) {
        util.abort(config.ERROR.tooMuchArguments);
    }

    var input_dir = process.cwd();

    if( arguments.length == 1 ) {
        input_dir = fs.realpathSync(arguments[0]);
    }

    var manifest = config.manifest;

    manifest.embed = this.embed || manifest.embed;
    manifest.compile = manifest.embed === true || this.compile || manifest.compile;
    manifest.entry_point = this.entry || manifest.entry_point;
    manifest.entry_point = './' + path.relative(input_dir, manifest.entry_point);
    manifest.build_dir = this.out || manifest.build_dir;
    manifest.build_dir = path.relative(input_dir,manifest.build_dir);
    manifest.extra = this.extra || manifest.extra;

    util.log('generating manifest file...');

    input_dir = path.join(input_dir,'manifest.json');
    fs.writeFileSync(input_dir,JSON.stringify(config.manifest,null,4),'utf-8');

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

    if( typeof output_dir == 'undefined' ){
        output_dir = path.resolve(input_dir,manifest.build_dir);
    }

    var needs_compile = false;
    if(manifest.compile || manifest.embed){// || manifest.entry_point != config.manifest.entry_point ){
        needs_compile = true;
    }

    mkdir.sync(output_dir);

    util.log('Generating package...');

    var content = JSON.stringify(pack.generate(input_dir,manifest));
    var gyp_path = path.join(__dirname,'../node.gyp');

    if(needs_compile && manifest.embed){
    
        var tmp_dir = path.join(output_dir,'.temp');
        var resource_path = path.join(tmp_dir,'resource.json');

        content = 'module.exports = ' + content;

        generate_node_gyp(gyp_path,[resource_path]);
        generate_resource(tmp_dir,content);

        compile_node(function(){
            copy_binary();
        });

    } else {

        generate_node_gyp(gyp_path);
        generate_resource(resource_path,content);

        copy_binary(output_dir,manifest.appname);
        
    }

}

var help = exports.help = function(){
  console.log(config.HELP);
}


function generate_resource(output_dir,content){
    var resource_path = path.resolve(output_dir,'resources.json');
    fs.writeFileSync(resource_path,content);
}

function generate_node_gyp(out_dir,extraLibraries){

    var node_gyp = fs.readFileSync(path.join(__dirname,'../node-gyp-template'));

    if( extraLibraries && extraLibraries.length ){

        node_gyp = JSON.parse(node_gyp.toString());

        extraLibraries.forEach(function(resource_file){
            node_gyp.variables.library_files.push(resource_file);
        });
    }

    node_gypfile = JSON.stringify(node_gyp);

    gyp_path = path.join(out_dir,'node.gyp');
    mkdir.sync(out_dir);
    fs.writeFileSync(gyp_path,node_gypfile);

    return node_gyp;
}

function copy_binary(output_dir,to_name){

    //@TODO copy binary

}

function compile_node(cb){
    var src_dir = path.resolve(__dirname,'../deps/node');

    var make = function (){
        exec('./configure',{
            cwd: path.resolve(__dirname,'../')
        },function(error,stdout){

            if(error !== null){
                util.log(error,'error');
            } else {

                util.log('Compiling...');

                exec('make',{
                    cwd: path.resolve(__dirname,'../')
                },function(error,stdout,stderr){

                    if(error !== null){
                        util.log(error,'error');
                    } else {

                        cb();

                    }
                });

            }

        }); 
    }

    if ( !fs.existsSync(src_dir) ){
        downloadNode(make);
    } else {
        make();
    }
}

function downloadNode(cb){

    var request = require('request')
      , zlib = require('zlib')
      , tar = require('tar')
      , distUrl = 'http://nodejs.org/dist'
      , version = '0.6.14';

    var tarballUrl = distUrl + '/v' + version + '/node-v' + version + '.tar.gz'
      , badDownload = false
      , gunzip = zlib.createGunzip()
      , srcDir = path.join(__dirname,'../deps/node')
      , extracter = tar.Extract({ path: srcDir, strip: 1 });


    var handler = function(err,res){
        if ( badDownload || err || res.statusCode != 200 ) {
            badDownload = true;
            cb(err || new Error(res.statusCode + ' status code downloading tarball'));
        } else {
            cb();
        }
    }
    
    gunzip.on('error', handler)
    extracter.on('error', handler)
    extracter.on('end', handler)

    download(tarballUrl,handler);
}

function download(url,onError) {
    var request = require('request');

    util.log('Downloading node tarball...');

    var requestOpts = {
        uri: url,
        onResponse: true
    }

    // basic support for a proxy server
    var proxyUrl = process.env.http_proxy
                || process.env.HTTP_PROXY
                || process.env.npm_config_proxy
    if (proxyUrl) {
      util.log('using proxy: ' + proxyUrl,'verbose');
      requestOpts.proxy = proxyUrl
    }
    return request(requestOpts, onError)
}
