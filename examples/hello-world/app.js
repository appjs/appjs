var app = module.exports = require('appjs');

app.serveFilesFrom(__dirname + '/content');

var window = app.createWindow({
  width  : 640,
  height : 460,
  icons  : __dirname + '/content/icons'
});

var menu = app.createMenu([{
  label:'_File',
  submenu:[
    {
      label:'E_xit',
      action: function(){
        window.close();
      }
    }
  ]
},{
  label:'_Edit',
  submenu:[
    {
      label:'Undo'
    },{
      label:''//separator
    },
    {
      label:'Copy',
      action:function(item){
        console.log("item "+item.label+" clicked");
      }
    },
    {
      label:'Paste',
      action:function(item){
        console.log("item "+item.label+" clicked");
      }
    }
  ]
}]);

window.on('create', function(){
  console.log("Window Created");
  window.frame.show();
  window.frame.center();
  window.frame.setMenuBar(menu);
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
