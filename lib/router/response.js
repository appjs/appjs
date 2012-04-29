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

    callback(status,status_text[status],mime_type,data);
  }

  return this;

}

module.exports = Response;