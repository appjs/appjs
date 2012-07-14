var  app = require('../../index.js')
  , Github = require('github');

var github = new Github({version: '3.0.0',debug:true})
  // get screen dimension
  , screenWidth = app.screenWidth()
  , screenHeight = app.screenHeight()

var windowSettings = {
  width: 460,
  height: 640,
  left: -1, // remove x and y if you want to put window at center, -1 means the same
  top: -1, // remove x and y if you want to put window at center, -1 means the same
  autoResize: false, // set to true if you want to change screen size dynamically using html elements
  resizable: false, // prevent users from changing screen width or height
  showChrome: true, // show border and title bar
  opacity:1, // you can set opacity of window.
  fullscreen:false, // we don't need fullscreen window
  showResizeGrip:false, // resize grip is an annoying triangle at the right bottom corner of window
  disableSecurity:true, // allow cross origin requests
  icons: {
    smaller: './assets/icons/16.png', //16x16
    small: './assets/icons/32.png', // 32x32
    big: './assets/icons/48.png', // 48x48
    bigger: './assets/icons/64.png' // 64x64 or 128x128
  }
};

/**
 * Create a new window. appjs:// is a special scheme for appjs requests.
 * Use this scheme to communicate with nodejs. Use appjs routers to handle
 * these special requests. Note that there is no http server in between.
 **/
var window = app.createWindow("http://appjs/",windowSettings);

window.on("create",function(){
  console.log("Window created");
  window.show();
});

window.on("ready",function(){
  console.log("Window is ready");
});

window.on("close",function(){
  console.log("Window closed");
});

app.on("exit",function(){
  console.log("Event Exit called");
});

app.use(app.staticRouter('./assets'));

app.post('/login',function(req,res,next){
  var username = req.post('username')
    , password = req.post('password');

  github.authenticate({
    type:'basic',
    username: username,
    password: password
  });

  github.user.get({}, function(err, result) {
      if(err) {
        res.send(401,{error:'Login Failed. Try Again.'});
        return;
      }

      res.send(200,result);
  });

});

app.get('/followers',function(req,res,next){
  github.user.getFollowers({user:req.param('user')},function(err,result){
    if(err) {
      res.send(200,{error:'Could not get followers list'});
      return;
    }

    res.send(200,result);
  })
});

app.get('/following',function(req,res,next){
  github.user.getFollowing({user:req.param('user')},function(err,result){
    if(err) {
      res.send(200,{error:'Could not get following list'});
      return;
    }

    res.send(200,result);
  })
});
