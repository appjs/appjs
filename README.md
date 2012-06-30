# AppJS

AppJS is an SDK to develop desktop applications using Node.js melded with Chromium. With AppJS you can develop desktop tools and applications using the same libraries and knowledge used to build websites. You get all the following in one package:

* JS, HTML5, CSS, SVG, WebGL provided by Chromium
* mature http/https servers and client APIs - Node
* filesystem, dns, cryptography, subprocesses, OS APIs - Node
* sandboxed code execution environements virtual machines - Node
* tools for exposing native C++ bindings to JavaScript--APIs for authoring and the tools for compiling - Node


---

* __Site: [appjs.org](http://appjs.org)__
* __Community: [mailing list](https://groups.google.com/forum/#!forum/appjs-dev)__

---

__AppJS is under heavy development. Expect many API changes and things to break.__

## Quick Start

Binary packages are available for Windows, Linux, and OS X (not for all architectures yet). These packages include everything needed to get started with AppJS, including a double clickable launcher. These packages include Node in them, they are zero dependency builds.

AppJS 0.0.13 Complete Distributables:

* [Windows](http://dists.appjs.org/0.0.13/appjs-0.0.13-win32-ia32.zip) (32 bit) -> launch.exe
* [Linux](http://dists.appjs.org/0.0.13/appjs-0.0.13-linux-x64.tar.gz) (64 bit) -> launch.sh
* [Mac](http://dists.appjs.org/0.0.13/appjs-0.0.13-darwin-ia32.zip) (32 bit) -> launch.sh

![Hello World](https://github.com/milani/appjs/raw/master/examples/output.jpg "Hello World")

## Node Install

__Due to the complex dependencies of AppJS installation via npm is more difficult than we'd like, be forewarned.__

You need to have node **v0.8.0** and node-gyp installed. After [installing node](https://github.com/joyent/node/wiki/Installation), use the following command to install node-gyp:

    $ sudo npm install node-gyp -g

Please take a look at [node-gyp repository](https://github.com/TooTallNate/node-gyp/) and make sure you have all required packages.


## Platform build requirements:

* __Mac OS X__: Currently 32bit node only, `$ node -e "console.log(process.arch)"` -> __ia32__
* __Linux__: `$ sudo apt-get install libgtk2.0-dev`
* __Windows__: Windows SDK, DirectX SDK, and Visual C++ 2010

## Usage Overview
There is a complete example in the `examples` folder.

```javascript
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
```


## License
( The MIT License )

Copyright (c) 2012 Morteza Milani and other AppJS contributors

See the LICENSE file for details.