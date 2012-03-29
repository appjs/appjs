var fs = require('fs');
var path = require('path');

try {
  var resource = JSON.parse(fs.readFileSync(path.join(process.cwd(),'./resources.json')));
} catch(e) {
  var resource = '';
}

exports.getEntry = function(path){

  if(typeof resource[path].content == 'string')
    resource[path].content = new Buffer(resource[path].content);
  
  return resource[path];
}
