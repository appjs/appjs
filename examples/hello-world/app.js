var app = module.exports = require('appjs');

app.serveFilesFrom(__dirname + '/content');


// Logger is enabled by default and it logs to process.stdout

// Uncomment this line to write the log in the given file
// app.logger.setFile( "./appjs.log");

// Set the loglevel to debug
app.logger.level = 'debug';

// This sets the format of the output string
app.logger.format = function(level, date, message){
  return "level: " + level + "\ndate: " + date + "\nmessage: " + message + "\n====";
};


var window = app.createWindow(null, {
  width  : 640,
  height : 460,
  icons  : __dirname + '/content/icons'
});

window.on('create', function(){
  console.log("Window Created");
  window.frame.show();
  window.frame.center();
});

window.on('ready', function(){
  console.log("Window Ready");
  window.process = process;
  window.module = module;

  function F12(e){ return e.keyIdentifier === 'F12' }
  function Command_Option_J(e){ return e.keyCode === 74 && e.metaKey && e.altKey }

  window.addEventListener('keydown', function(e){
    if (F12(e) || Command_Option_J(e)) {
      window.frame.openDevTools();
    }
  });
});

window.on('close', function(){
  console.log("Window Closed");
});


app.logger.info("This is the info message");

app.logger = false;

app.logger.info( "This message will not appear" );