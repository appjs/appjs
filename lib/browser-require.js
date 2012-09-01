module.exports = browserRequire;

function browserRequire(window, request, callback){
  window.Function('return '+function(require, process, Buffer, global, request, callback){
    var Module = require('module'),
        path   = require('path');

    var browser = { compile: compileInBrowser,
                    cache: {} };

    var node    = { compile: Module.prototype._compile,
                    cache: Module._cache };


    Module.prototype._compile = function(content, filename){
      var moduleSystem = executingInBrowser ? browser : node;
      Module._cache = moduleSystem.cache;
      var ret = moduleSystem.compile.call(this, content, filename);
      Module._cache = moduleSystem.cache;
      return ret;
    };

    var executingInBrowser = true;
    try {
      var result = require(request);
      if (!document.mainModule)
        document.mainModule = result;
      callback(null, result);
    } catch (e) {
      callback(e);
    }
    executingInBrowser = false;

    function compileInBrowser(content, filename) {
      var self = this;

      function require(request, runInNode){
        executingInBrowser = !runInNode;
        request = self.require(request);
        executingInBrowser = false;
        return request;
      }

      function resolve(request){
        return Module._resolveFilename(request, self);
      }

      require.extensions = Module._extensions;
      require.cache      = Module._cache;
      require.resolve    = resolve;
      require.main       = document.mainModule;

      content = content.replace(/^\#\!.*/, '');

      var
       dirname = path.dirname(filename),
       module  = this,
       exports = this.exports;

       args  = ['exports','require','module','__filename','__dirname','Buffer','process','global',content],
       argvs = [ exports,  require,  module,    filename,    dirname,  Buffer,  process,  global];

      try {
        return Function.apply(null, args).apply(exports, argvs);
      } catch (e) {
        console.log(e.stack);
      }
    }
  })().call(window, require, process, Buffer, window, request, callback || function(){});
}
