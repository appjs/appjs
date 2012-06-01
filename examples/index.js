var appjs = require('../index.js');

var settings = {
  width: 800,
  height: 600,
  autoResize:false,
  resizable:true,
  showChrome: true,
  opacity:1,
  fullscreen:false,
  showResizeGrip:false,
  disableSecurity:true
};

var window;
var app = appjs.init();

app.on("window_ready",function(){
  console.log("Event WindowReady called");

  window.show();

});

app.on("window_close",function(){
  console.log("Event WindowClose called");
});

app.on("show",function(){
  console.log("Event Show called");
});

app.on("ready",function(){
  console.log("Event Ready called");

  window.runInBrowser(function(){
    var body = document.body;
    body.style.color="#f60";
    console.log("I'm in the browser context!!!");  
  });

});

app.use(app.staticRouter('./'));

app.post('/save',function(req,res,next){
  res.send(200,'Hey! You sent a post request. Take a look at console');
  console.log('Hi '+req.post('firstname'));
});

window = app.createWindow("appjs://app/",settings);

