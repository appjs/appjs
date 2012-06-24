var mime = require('mime')
  , path = require('path')
  , fs = require('fs');

function staticRouter(rootDir){
  
  var root = path.join(process.cwd(),rootDir);

  var router = function(req,res,next){
    if(req.method == "GET") {

      urlPath = req.pathname;

      if(urlPath == '/') urlPath = '/index.html';
      if(urlPath.charAt(urlPath.length-1) == '/') urlPath += 'index.html';

      var filePath = path.join(root,urlPath);

      fs.stat(filePath,function(err,stat){

        if( err || !stat.isFile() ){
          next();
          return;
        }

        fs.readFile(filePath,function(err,content){
          
          if(err) {
            if(err.errno == 34)
              res.send(404,'File Not Found');
            else
              res.send(500,'Internal Server Error');
            res.sent = true;
            return;
          }

          mime_type = mime.lookup(filePath);
          res.send(200,mime_type,content);
          res.sent = true;
        })

      })
    } else {
      next();
    }
  };

  return router;

}

module.exports = staticRouter;
