# AppJS

AppJS is an SDK to develop desktop applications using node. Or at least
it is going to be!

The goal is to facilitate native app development using
tools every developer is familiar with nowadays: HTML/CSS/JS

**Attention:** AppJS is under heavy development. API changes a lot until we bump version to v0.1.0

![Hello World](https://github.com/milani/appjs/raw/master/examples/output.png "Hello World")

## Preinstall

You need to have node-gyp installed:

    $ npm install node-gyp -g

Please take a look at [node-gyp repository](https://github.com/TooTallNate/node-gyp/) 
and make sure you have all required packages.

### Linux users

You should have libgtk2.0-dev installed. Ubuntu users can install it using the following command:

    sudo apt-get install libgtk2.0-dev

## How to install

** Only Linux and Windows ports work for now. Pull requests for Mac OS are very welcome **

Using npm:

    $ npm install appjs

Using git:

    $ git clone https://github.com/milani/appjs
    $ cd appjs
    $ npm install -d

### Windows users

You should copy all shared libraries ( .dll files ) located at `appjs/deps/cef/Release`
to current working directory or AppJS fails to run with error: module not found.

## How to use

There is a complete example in the `examples/` folder.

Here is a simple one:

    var appjs = require('appjs');

    var window;

    // Initialize appjs.
    var app = appjs.init();

    // Called when page load finishes.
    app.on("ready",function(){
        console.log("Event Ready called");

        // Runs a script in browser context.
        window.runInBrowser(function(){
            var body = document.body;
            body.style.color="#f60";
        });

      // Show created window ( see below )
      window.show();
    });

    // Routing:

    // you can use a static router:
    // app.use(app.staticRouter('./public'));

    // or you can handle requests manually:
    app.get("/",function(req,res,next){
      res.send("\
        <html>\
            <head><title>Hello World</title></head>\
            <body>Hello World</body>\
        </html>\
      ")
    });

    // Creates a new window. Its invisible until window.show() get called.
    // http://appjs/ is a special url. It is home for your application!
    window = app.createWindow("http://appjs/",{autoResize:false});

## Thanks to

* Brandon Benvie: He helps me with CEF.
* Ali Farhadi: He helps me with AppJS.

## License
( The MIT License )

Copyright (c) 2012 Morteza Milani and other AppJS contributors

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
