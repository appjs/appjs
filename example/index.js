var appjs = require('../index.js');
var util = require('util');

var settings = {
  width: 800,
  height: 200,
  resizable:true,
  showChrome: true,
  opacity:1,
  fullscreen:false,
  showResizeGrip:false,
  entryPoint:"appjs://app/"
};

var window;
var e = appjs.init();

e.on("window_ready",function(){
  console.log("Event WindowReady called");
  
  window.runInBrowser(function(){
    console.log("I'm in the browser context!!!");
  });

  window.show();

});

e.on("window_close",function(){
  console.log("Event WindowClose called");
});

e.on("show",function(){
  console.log("Event Show called");
});

e.on("ready",function(){
  console.log("Event Ready called");
  window.runInBrowser(function(){
    var body = document.body;
    body.style.color="#f60";
  });

});

e.on("route",function(req,callback){
  console.log("Router called with request: "+ req.method + " " +req.url);
  callback("text/html",
    "<html><head><title>Hello world</title></head><body>Hello World</body></html>");

});

window = e.createWindow(settings);

