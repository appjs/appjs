var app = module.exports = require('appjs');

app.get('/',function(req,res,next){
  res.send("Hello World!");
});

app.serveFilesFrom('./content');  // serves files to browser requests to "http://appjs/*"

var window = app.createWindow("http://appjs/", {
  width           : 640,
  height          : 460,
  left            : -1,    // optional, -1 centers
  top             : -1,    // optional, -1 centers
  autoResize      : false, // resizes in response to html content
  resizable       : false, // controls whether window is resiable by user
  showChrome      : true,  // show border and title bar
  opacity         : 1,     // opacity from 0 to 1 (Linux)
  alpha           : false, // alpha compositing based on window content (Windows & Mac)
  fullscreen      : false, // covers whole screen and has no chrome
  disableSecurity : true,  // allow cross origin requests
});


window.on("create",function(){
  console.log("Window Created");
});

window.on("ready",function(){
  console.log("Window Ready");
  this.show();
});

window.on("close",function(){
  console.log("Window Closed");
});
