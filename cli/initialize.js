/**
 * Set the title.
 */

process.title = 'appjs-initialize';

/**
 * Necessary modules and variables
 */

var util     = require('../cli/util')
  , fs       = require('fs')
  , path     = require('path')
  , platform = require('os').platform()
  , version  = require('../package.json').version
  , arch     = require('os').arch()
  , depsDir  = path.join(__dirname,'../build/Release')
  , env      = process.env.NODE_ENV || 'Debug';

/**
 * Starts installing CEF libraries
 */

function install(appjs_version) {

  downloadAppJS(appjs_version,function(err){

    if(err) {
      util.log('Failed to add dependencies','error');
      throw err;
    }

    util.log('Done!');
  });

}

/**
 * Downloads CEF binary package according to
 * developer's platform and extracts it to
 * deps/cef
 */

function downloadAppJS(version,cb){

    var zlib = require('zlib')
      , tar = require('tar')
      , distUrl = 'https://github.com/downloads/milani/appjs'
      , fileName = 'appjs_' + version + '_' + platform + '_' + arch + '.tar.gz';

    var tarballUrl = distUrl + '/' + fileName
      , gunzip     = zlib.createGunzip()
      , extracter  = tar.Extract({ path: depsDir });

    var errorHandler = function(err,res){
        if ( err || res.statusCode != 200 ) {
            cb(err || new Error(res.statusCode + ' status code downloading tarball'));
        }
    }

    gunzip.on('error', errorHandler)
    extracter.on('error', errorHandler)
    extracter.on('end', function(){

        if(platform == 'win32') {
          
          try {
            fs.rmdirSync( path.join(depsDir,'cef') );
          } catch(e) {};

          fs.rename(path.join(depsDir,fileName.replace('.tar.gz','')), path.join(depsDir,'cef'),cb);
        } else {
          
          try {
            fs.unlinkSync( path.join(depsDir,'cef') );
          } catch(e) {};

          fs.symlink( path.join(depsDir,fileName.replace('.tar.gz','')), path.join(depsDir,'cef') ,cb);
        }
    })

    download(tarballUrl,errorHandler)
      .pipe(gunzip)
      .pipe(extracter);
}

/**
 * Returns a Request object
 */

function download(url,onError) {
    var request = require('request');

    util.log('Downloading necessary appjs files for '+platform+'-'+arch+'...');

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
    return request(requestOpts).on('error',onError).on('response',function(res){onError(null,res)});
}

/**
 * Start installing appjs.node
 */
 
install(version);
