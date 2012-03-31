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

    var manifest = loadManifest(input_dir, this);

    if( typeof output_dir == 'undefined'){
        output_dir = path.resolve(input_dir,manifest.build_dir);
    }

    mkdir.sync(output_dir);
    generate_resource(input_dir, output_dir, manifest);

    util.log('Done.','success');
}

var build = exports.build = function(){

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

    var manifest = loadManifest(input_dir, this);

    if( typeof output_dir == 'undefined' ){
        output_dir = path.resolve(input_dir,manifest.build_dir);
    }

    var needs_compile = manifest.compile || manifest.embed; // || manifest.entry_point != config.manifest.entry_point 

    mkdir.sync(output_dir);

    if(needs_compile){
    
        var resource_path = generate_resource(input_dir, output_dir, manifest);
        generate_node_gyp([resource_path]);
        compile_node(function(){
            if (manifest.embed) {
                fs.unlink(resource_path);
            }
            copy_binary(output_dir, manifest.appname);
        });

    } else {

        generate_resource(input_dir, output_dir, manifest);
        generate_node_gyp();
        copy_binary(output_dir,manifest.appname);
        
    }

}

var help = exports.help = function(){
  console.log(config.HELP);
}

function loadManifest(manifest_path, conf) {
    util.log('Reading manifest file...');

    try {
        var manifest = JSON.parse(fs.readFileSync(path.join(manifest_path,'manifest.json')));
    } catch (e) {
        util.abort('Manifest file not found.');
    }
    
    manifest.compile = conf.compile || manifest.compile;
    manifest.embed = conf.embed || manifest.embed;
    manifest.entry_point = conf.entry || manifest.entry_point;
    //manifest.build_dir = output_dir || conf.out || manifest.build_dir;
    manifest.extra = conf.extra || manifest.extra;
    return manifest;
}

function generate_resource(input_dir, output_dir, manifest){
    util.log('Generating package...');
    var content = JSON.stringify(pack.generate(input_dir,manifest));
    var resource_path = path.resolve(output_dir,'resources.json');
    if (manifest.embed) {
        content = 'module.exports = ' + content;
    }
    fs.writeFileSync(resource_path,content);
    return resource_path;
}

function generate_node_gyp(extraLibraries){

    var node_gyp = fs.readFileSync(path.join(__dirname,'../node-gyp-template'));

    if( extraLibraries && extraLibraries.length ){

        node_gyp = JSON.parse(node_gyp.toString());

        extraLibraries.forEach(function(resource_file){
            node_gyp.variables.library_files.push(resource_file);
        });
    }

    node_gypfile = JSON.stringify(node_gyp);

    gyp_path = path.join(__dirname,'../node.gyp');
    fs.writeFileSync(gyp_path,node_gypfile);

    return node_gyp;
}

function copy_binary(output_dir,to_name){
    var from = fs.createReadStream(path.join(__dirname, '../node'));
    var to = fs.createWriteStream(path.join(output_dir, to_name));
    from.pipe(to).on('close', function() {
        util.log('Binary file created.');
    });
}

function compile_node(cb){
    var src_dir = path.resolve(__dirname,'../deps/node/src');

    var make = function (err){

        if( err ){
            util.abort(err);
        }

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

    if ( !path.existsSync(src_dir) ){
        downloadNode(make);
    } else {
        make();
    }
}

function downloadNode(cb){

    var zlib = require('zlib')
      , tar = require('tar')
      , distUrl = 'http://nodejs.org/dist'
      , version = '0.6.14';

    var tarballUrl = distUrl + '/v' + version + '/node-v' + version + '.tar.gz'
      , gunzip = zlib.createGunzip()
      , srcDir = path.join(__dirname,'../deps/node')
      , extracter = tar.Extract({ path: srcDir, strip: 1 });


    var errorHandler = function(err,res){
        if ( err || res.statusCode != 200 ) {
            cb(err || new Error(res.statusCode + ' status code downloading tarball'));
        }
    }

    gunzip.on('error', errorHandler)
    extracter.on('error', errorHandler)
    extracter.on('end', cb)

    download(tarballUrl,errorHandler)
      .pipe(gunzip)
      .pipe(extracter);
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
