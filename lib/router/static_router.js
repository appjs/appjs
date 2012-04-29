var mime = require('mime')
  , path = require('path')
  , fs = require('fs');

function staticRouter(rootDir){
  
  var root = path.join(process.cwd(),rootDir);

  var router = function(req,res,next){

    if(req.method == "GET") {

      urlPath = req.pathname;

      if(urlPath == '/') urlPath = '/index.html';

      var filePath = path.join(root,urlPath);

      fs.stat(filePath,function(err,stat){

        if( err || !stat.isFile() ){
          next();
          return;
        }

        fs.readFile(filePath,function(err,content){
          
          if(err) {
            res.send(500,'Internal Server Error');
            return;
          }

          mime_type = mime.lookup(filePath);

          res.send(200,mime_type,content.toString());

        })

      })
    } else {
      next();
    }
  };

  return router;

}

module.exports = staticRouter;