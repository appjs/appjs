# AppJS

AppJS is an SDK to develop desktop applications using node. Or at least
it is going to be!

The goal is to facilitate native app development using
tools every developer is familiar with nowadays: HTML/CSS/JS

**Attention:** AppJS is under heavy development. API changes a lot until we bump version to v0.1.0

![Hello World](https://github.com/milani/appjs/raw/master/examples/output.png "Hello World")

## Preinstall

You need to have node **v0.8.0** and node-gyp installed.
After [installing node](https://github.com/joyent/node/wiki/Installation), use the following command
to install node-gyp:

    $ sudo npm install node-gyp -g

Please take a look at [node-gyp repository](https://github.com/TooTallNate/node-gyp/) 
and make sure you have all required packages.

### Mac OS X users

AppJS only works with 32bit node. Please make sure by running:

    $ node -e "console.log(process.arch)"
    ia32

## How to install

There are ready to use appjs packages that do not need compilation.
They contain node binary file, a launcher and a simple appjs app to
help you get start.

Download them from http://appjs.org/#download

You can also use npm to install these ready to use packages easily.
This way you only get appjs module, it doesn't include launcher.

    $ npm install appjs

You can clone appjs repository and build it on your own. You need to have
the following dependencies installed:

### Linux users

    $ sudo apt-get install libgtk2.0-dev

### Windows users

You need Windows SDK and Visual Studio 2010. Take a look at node-gyp readme
to get more information.

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

If you are using appjs tarballs, you can run the boilerplate application simply
by clicking launch.sh or launch.exe.

## Thanks to

* Brandon Benvie: He helps me with CEF.
* Ali Farhadi: He helps me with AppJS.
* And all other AppJS contributors.

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
