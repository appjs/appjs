var npm          = require('npm'),
    errorHandler =require('npm/lib/utils/error-handler'),
    platform     = process.platform,
    arch         = process.arch,
    dependency   = 'appjs-' + platform;

if(platform == "linux") {
    dependency += '-' + arch;
}

npm.load({},function(){
    npm.commands.install([dependency],errorHandler);
})