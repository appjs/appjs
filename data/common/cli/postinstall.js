var path         = require('path'),
    //errorHandler = require('npm/lib/utils/error-handler'),
    exec         = require('child_process').exec,
    platform     = process.platform,
    arch         = process.arch,
    dependency   = 'appjs-' + platform;

if (platform == "linux") {
  dependency += '-' + arch;
}

console.log('Installing '+dependency+' pacakge...');

exec('npm install '+dependency,{cwd:path.resolve(__dirname,'../../../')},function(err,stdout,stderr){
  
  if(err) {
    console.log('Error installing '+dependency);
    console.log('Please use `npm install '+dependency+' and install the package manually');
  } else {
    console.log(stderr);
  }

});
/*npm.load({}, function(){
  npm.commands.install([dependency], errorHandler);
});*/

