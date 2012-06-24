var mime = require('mime')
  , path = require('path')
  , fs = require('fs');

var status_text = {
  200:'OK',
  304:'Not Modified',
  400:'Bad Request',
  401:'Unauthorized',
  403:'Forbidden',
  404:'Not Found',
  500:'Internal Server Error'
};

function Response(callback) {
  this.sent = false;
  this.send = function send ( status, mime_type, data ) {

    if(arguments.length == 1) {
      if(typeof status == 'number') {
        mime_type = 'text/plain';
        data = '';
      } else {
        mime_type = 'text/html';
        data = status;
        status = 200;
      }
    } else if(arguments.length == 2){
      data = mime_type;
      mime_type = 'text/plain';
    }

    if(typeof data == 'object' && !(data instanceof Buffer) ) {
      mime_type = 'application/json';
      data = JSON.stringify(data);
    }

    if(!(data instanceof Buffer)) {
      data = new Buffer(data);
    }

    callback(status,status_text[status],mime_type,data);
    this.sent = true;
  }

/**
 * Method to send file content to client
 * (eg: app.post() + static_router)
 * 
 * @param {Object} options
 * @api private
 */

  this.sendFile = function ( status, file ) {

    var filePath = path.join(process.cwd(), file);
    fs.stat(filePath,function(err,stat){
      if( err || !stat.isFile() ){
        if(err.errno == 34)
          callback(404,status_text[404],'text/plain', new Buffer('Page Not Found ['+file+']'));
        else
          callback(500,status_text[500],'text/plain', new Buffer('Internal Server Error ['+path+']'));
        return;
      }

      fs.readFile(filePath,function(err,content){
        
        if(err) {
          if(err.errno == 3)
            callback(403,status_text[403],'text/plain', new Buffer(status_text[403]+' ['+path+']'));
          else
            callback(500,status_text[500],'text/plain', new Buffer('Internal Server Error ['+path+']'));
          return;
        }

        mime_type = mime.lookup(filePath);
        callback(status,status_text[status],mime_type,content);
      })

    })
    this.sent = true;
  }

  return this;

}

module.exports = Response;
