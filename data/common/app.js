var app  = module.exports = require('appjs'),
    path = require('path');

var content = path.resolve(__dirname, 'content'),
    icons   = path.join(content, 'icons');

// serves files to browser requests to "http://appjs/*"
app.serveFilesFrom(content);

var window = app.createWindow('http://appjs/', {

/******** defaults
* autoResize       : false, // resizes in response to html content
* showChrome       : true,  // show border and title bar
* opacity          : 1,     // flat opacity for whole window, percentage expressed by 0 to 1
* alpha            : false, // per-pixel alpha blended background (Windows & Mac)
* fullscreen       : false, // client area covers whole screen, no chrome
* left             : -1,    // centered by default
* top              : -1,    // centered by default
*********/

  width            : 640,
  height           : 460,
  resizable        : false, // controls whether window is resizable by user
  disableSecurity  : true,  // allow cross origin requests
  icons: { smaller : icons + '/16.png',
           small   : icons + '/32.png',
           big     : icons + '/64.png',
           bigger  : icons + '/128.png' }
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
