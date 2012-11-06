/**
* log any uncaught exceptions to disk in case user is not running from console.
*/
process.on('uncaughtException',function(e) {
	require('fs').writeFile("error.log", e.message+"\n"+e.stack, function(err) {
		if(err) {
			console.log("error writing error log:",err);
		} else {
			console.log("uncaughtException:",e.stack);
		}
	}); 
});

var    fs = require('fs')
	,path = require('path')
	 ,app = require('appjs')
;

if (process.argv.length>2) {
	var appPackage = require('appjs-package');
	appPackage.getPackageInfo(process.argv[2],function(err,pInfo) {
		if (err) throw err;
		if (pInfo.isPackage) {
			//serve files using the package router.
			app.router.use(pInfo.router);
		}
		app.readPackageFile = pInfo.readPackageFile;
		appPackage.launch(pInfo);
	});
} else {
	app.serveFilesFrom(__dirname + '/content');

	var menubar = app.createMenu([{
	  label:'&File',
	  submenu:[
		{
		  label:'E&xit',
		  action: function(){
			window.close();
		  }
		}
	  ]
	},{
	  label:'&Window',
	  submenu:[
		{
		  label:'Fullscreen',
		  action:function(item) {
			window.frame.fullscreen();
			console.log(item.label+" called.");
		  }
		},
		{
		  label:'Minimize',
		  action:function(){
			window.frame.minimize();
		  }
		},
		{
		  label:'Maximize',
		  action:function(){
			window.frame.maximize();
		  }
		},{
		  label:''//separator
		},{
		  label:'Restore',
		  action:function(){
			window.frame.restore();
		  }
		}
	  ]
	}]);

	menubar.on('select',function(item){
	  console.log("menu item "+item.label+" clicked");
	});

	var trayMenu = app.createMenu([{
	  label:'Show',
	  action:function(){
		window.frame.show();
	  },
	},{
	  label:'Minimize',
	  action:function(){
		window.frame.hide();
	  }
	},{
	  label:'Exit',
	  action:function(){
		window.close();
	  }
	}]);

	var statusIcon = app.createStatusIcon({
	  icon:'./data/content/icons/32.png',
	  tooltip:'AppJS Hello World',
	  menu:trayMenu
	});

	var window = app.createWindow({
	  width  : 640,
	  height : 460,
	  icons  : __dirname + '/content/icons'
	});

	window.on('create', function(){
	  console.log("Window Created");
	  window.frame.show();
	  window.frame.center();
	  window.frame.setMenuBar(menubar);
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

}