var app = require('appjs');

var window = app.createWindow();

window.on('create',function(){
  this.frame.show();
});
