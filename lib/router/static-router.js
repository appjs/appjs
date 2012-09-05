var mime = require('mime'),
    path = require('path'),
    fs = require('fs');

module.exports = staticRouter;

function staticRouter(root){
  root = path.join(root);

  return function router(request, response, next){
    if (request.method === 'get') {
      var url = request.pathname === '/' ? '/index.html' : request.pathname;
      var filePath = path.join(root, url);
      fs.stat(filePath, function(err, stat){
        if (err || !stat.isFile()) {
          next()
        } else {
          response.headers.setHeader('Last-Modified', stat.mtime.toUTCString());
          var mimetype = mime.lookup(filePath);
          fs.readFile(filePath, function(err, buffer){
            if (err) {
              response.send(500);
            } else {
              response.send(200,mimetype,buffer);
            }
          });
        }
      });
    } else {
      next();
    }
  };
}
