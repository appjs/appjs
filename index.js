var appjs = require('./build/Release/appjs.node');
var path = require('path');
var url = require('url');
var fs = require('fs');

appjs.staticRouter = function(rootDir){
  var root = path.join(process.cwd(),rootDir);
  
  var router = function(req,callback){
  	if(req.method == "GET") {

  	  var urlPath = url.parse(req.url).pathname;

  	  if(urlPath == '/') urlPath = '/index.html';

  	  var filePath = path.join(root,urlPath);

  	  if(fs.statSync(filePath).isFile()){
  	  	var fileContent = fs.readFileSync(filePath);
  	  	callback("text/html",fileContent);
  	  	return;
  	  }
  	}
  	callback("text/html","404");
  };

  return router;

}

module.exports = appjs;