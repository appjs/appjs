/**
 * Set the title.
 */

process.title = 'appjs-configure'

/**
 * Necessary modules and variables
 */

var util     = require('../cli/util')
  , fs       = require('fs')
  , path     = require('path')
  , platform = require('os').platform()
  , arch     = require('os').arch()
  , depsDir  = path.join(__dirname,'../deps')
  , dataDir  = path.join(__dirname,'../data')
  , env      = process.env.NODE_ENV || 'Debug';

// TODO: remove me after CEF supports building under OSX as 64bit
if (platform === 'darwin' && arch === 'x64') {
  arch='ia32'
}

/**
 * Starts installing CEF libraries
 */

function install(cef_version) {

  downloadCef(cef_version,function(err){

    if(err) {
      util.log('Failed to add dependencies','error');
      throw err;
    }

    copyDllWrapper(function(err) {
      if (err) {
        util.log('Failed to copy dll_wrapper.gyp');
        throw err;
      }

      util.log('Done!');
    });
  });
}

function copyDllWrapper(fn) {
  var target = path.join(depsDir, 'cef', 'dll_wrapper.gyp');
  fs.exists(target, function(exists) {
    if (exists) {
      fs.unlinkSync(target);
    }

    fs.createReadStream(path.join(dataDir, 'common', 'dll_wrapper.gyp')).pipe(
      fs.createWriteStream(target)
    );
  });
}

/**
 * Downloads CEF binary package according to
 * developer's platform and extracts it to
 * deps/cef
 */

function downloadCef(version,cb){

    var zlib = require('zlib')
      , tar = require('tar')
      , distUrl = 'https://github.com/downloads/appjs/appjs'
      , version = version
      , dirname = 'cef_binary_' + version + '_' + platform + '_' + arch
      , fileName = dirname + '.tar.gz';

    var tarballUrl = distUrl + '/' + fileName
      , gunzip     = zlib.createGunzip()
      , extracter  = tar.Extract({ path: depsDir });

    var errorHandler = function(err,res){
        if ( err || res.statusCode != 200 ) {
            cb(err || new Error(res.statusCode + ' status code downloading tarball'));
        }
    };

    var finish = function() {
      if(platform == 'win32') {

        try {
          fs.rmdirSync( path.join(depsDir,'cef') );
        } catch(e) {};

        fs.rename(path.join(depsDir, dirname), path.join(depsDir,'cef'),cb);
      } else {

        try {
          fs.unlinkSync( path.join(depsDir,'cef') );
        } catch(e) {};

        fs.symlink( path.join(depsDir, dirname), path.join(depsDir,'cef') ,cb);
      }
    };

    gunzip.on('error', errorHandler)
    extracter.on('error', errorHandler)
    extracter.on('end', finish)

    fs.exists(path.join(depsDir, dirname), function(exists) {
      if (!exists) {
        download(tarballUrl,errorHandler)
          .pipe(gunzip)
          .pipe(extracter);
      } else {
        finish();
      }
    });
}

/**
 * Returns a Request object
 */

function download(url,onError) {
    var request = require('request');

    util.log('Downloading cef tarball for '+platform+'-'+arch+'...');

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
 * Start installing CEF version 1.1180.724
 */

install('1.1180.724');
