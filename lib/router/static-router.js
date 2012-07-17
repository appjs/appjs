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

      // NOTICE: Don't use request.sendFile because if the file is not there
      // we won't be able to call next(). Also do not fs.stat and
      // then request.sendfile since it causes two fs.stat that is not good.
      fs.stat(filePath, function(err, stat){
        if (err || !stat.isFile()) {
          next()
        } else {
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
