var app = module.exports = require('appjs');

app.serveFilesFrom(__dirname + '/content');

var window = app.createWindow({
  width  : 640,
  height : 460,
  icons  : __dirname + '/content/icons',
/***************************** defaults ********************************
* url            : 'http://appjs', // serve static file root and routers
* autoResize     : false,          // resizes in response to html content
* showChrome     : true,           // show border and title bar
* resizable      : false,          // control if users can resize window
* disableSecurity: true,           // allow cross origin requests
* opacity        : 1,              // flat percent opacity for window
* alpha          : false,          // per-pixel alpha blended (Win & Mac)
* fullscreen     : false,          // client area covers whole screen
* left           : -1,             // centered by default
* top            : -1,             // centered by default
*************************************************************************/
});

window.on('create', function(){
  console.log("Window Created");
  this.frame.show();
  this.frame.center();
});

window.on('ready', function(){
  console.log("Window Ready");
  this.require = require;
  this.process = process;
  this.module = module;
  this.console.log('process', process);
});

window.on('close', function(){
  console.log("Window Closed");
});
