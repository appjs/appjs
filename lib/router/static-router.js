var mime = require('mime'),
    path = require('path'),
    fs = require('fs');

module.exports = staticRouter;

function staticRouter(root){
  root = path.join(process.cwd(), root);

  return function router(request, response, next){
    if (request.method == 'GET') {
      var url = request.pathname === '/' ? '/index.html' : request.pathname;
      response.sendFile(path.join(root, url));
    } else {
      next();
    }
  };
}
