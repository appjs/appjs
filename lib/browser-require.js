module.exports = browserRequire;
var parent = module.parent.parent.parent.parent;

function browserRequire(window){

  function require(request){
    return parent.require(request);
  }

  window.Function('return '+function(_require, process, Buffer, global){
    var Module = _require('module'),
        path   = _require('path');

    var browser = { compile: compileInBrowser,
                    cache: {} };

    var node    = { compile: Module.prototype._compile,
                    cache: Module._cache };

    var executingInBrowser = false;

    Module.prototype._compile = function(content, filename){
      var moduleSystem = executingInBrowser ? browser : node;
      Module._cache = moduleSystem.cache;
      var ret = moduleSystem.compile.call(this, content, filename);
      Module._cache = moduleSystem.cache;
      return ret;
    };

    window.require = function require(request, runInNode){
      executingInBrowser = !runInNode;
      var result = _require(request);
      executingInBrowser = false;
      return result;
    };

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
        return e;
      }
    }
  })().call(window, require, process, Buffer, window);
}
