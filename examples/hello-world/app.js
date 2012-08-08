var app = module.exports = require('appjs');

app.serveFilesFrom(__dirname + '/content');

var window = app.createWindow({
  width  : 640,
  height : 460,
  icons  : __dirname + '/content/icons'
});

window.on('create', function(){
  console.log("Window Created");
  window.frame.show();
  window.frame.center();
});
var is_fullscreen = false;

window.on('ready', function(){
  console.log("Window Ready");
  window.require = require;
  window.process = process;
  window.module = module;
  window.addEventListener('keydown', function(e) {
    if (e.keyIdentifier === 'F11') {
      if (is_fullscreen === false) 
        window.frame.fullscreen();
      else
        window.frame.restore();
      is_fullscreen = !(is_fullscreen);
    } else if (e.keyIdentifier === 'F12') {
      window.frame.openDevTools();
    }
  });
});

window.on('close', function(){
  console.log("Window Closed");
});
