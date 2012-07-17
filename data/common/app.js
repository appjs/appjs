var app = module.exports = require('appjs');
var path = require('path');

// serves files to browser requests to "http://appjs/*"
app.serveFilesFrom(path.resolve(__dirname, 'content'));

var window = app.createWindow('http://appjs/', {
  width           : 640,
  height          : 460,
  left            : -1,    // optional, -1 centers
  top             : -1,    // optional, -1 centers
  autoResize      : false, // resizes in response to html content
  resizable       : false, // controls whether window is resizable by user
  showChrome      : true,  // show border and title bar
  opacity         : 1,     // opacity from 0 to 1 (Linux)
  alpha           : true,  // alpha composited background (Windows & Mac)
  fullscreen      : false, // covers whole screen and has no border
  disableSecurity : true   // allow cross origin requests
});


window.on('create', function(){
  console.log("Window Created");
});

window.on('ready', function(){
  this.require = require;
  this.process = process;
  this.module = module;
  this.console.log('process', process);
  this.frame.center();
  this.frame.show();
  console.log("Window Ready");
});

window.on('close', function(){
  console.log("Window Closed");
});
