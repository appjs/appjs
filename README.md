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

## 30 Second Quickstart
The below packages include everything needed to get started with AppJS, including Node.js, all dependencies, binaries, and a launcher ready to go out of the box. 1.) Extract to a folder. 2.) Double click on launch. 3.) Hello World.

__AppJS 0.0.13 Complete Distributables:__

* [Windows](http://dists.appjs.org/0.0.13/appjs-0.0.13-win32-ia32.zip) -> launch.exe
* Linux [32 bit](http://dists.appjs.org/0.0.13/appjs-0.0.13-linux-ia32.tar.gz) / [64 bit](http://dists.appjs.org/0.0.13/appjs-0.0.13-linux-x64.tar.gz) -> launch.sh
* [Mac](http://dists.appjs.org/0.0.13/appjs-0.0.13-darwin-ia32.zip) -> launch.sh


__AppJS 0.0.14 Beta Distributables:__

These are a work in progress.

* [Windows](http://dists.appjs.org/0.0.14/appjs-0.0.14b3-win32-ia32.zip) -> app.exe
* Linux [32 bit](http://dists.appjs.org/0.0.14/appjs-0.0.14-linux-ia32.tar.gz) / [64 bit](http://dists.appjs.org/0.0.14/appjs-0.0.14-linux-x64.tar.gz) -> launch.sh
* [Mac](http://dists.appjs.org/0.0.14/appjs-0.0.14-darwin-ia32.zip) -> launch.sh

![Hello World](https://github.com/milani/appjs/raw/master/examples/output.jpg "Hello World")

## One Minute Usage Overview
There is a complete example in the `examples` folder.

```javascript
var appjs = require('appjs');

// Creates a new window. Its invisible until window.show() get called.
// http://appjs/ is a special url. It is home for your application!
var window = app.createWindow("http://appjs/",{autoResize:false});

// Called when page load finishes.
window.on("ready",function(){
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
```

## Node Install
__Due to the complex dependencies of AppJS, installation via npm is difficult and error prone.__

* First you need [Node 0.8+](https://github.com/joyent/node/wiki/Installation) installed.
* Then use npm to install [node-gyp](https://github.com/TooTallNate/node-gyp): `npm install node-gyp`.
* Finally, install appjs: `npm install appjs`.

Currently the binaries require Node for the following architectures:

* Linux - 64 bit
* Mac - 32 bit
* Windows - 32 bit

## Platform build requirements:

* __Mac OS X__: Currently 32bit node only, `$ node -e "console.log(process.arch)"` -> __ia32__
* __Linux__: `sudo apt-get install libgtk2.0-dev`
* __Windows__: Windows SDK, DirectX SDK, and Visual C++ 2010

## Coming Soon
The follow are things we're currently working on solutions for and are high on the priority list to fix/address.

* bridging of Node and browser JS context so sharing of objects directly is possible
* distribution tools to help package up your apps for deployment
* expanded platform APIs to cover the standard use-cases for desktop apps

## A Preview

![Browser/Node Cross Context](https://github.com/milani/appjs/raw/master/examples/node-in-browser.png "Browser/Node Cross Context")


## License
( The MIT License )

Copyright (c) 2012 Morteza Milani and other AppJS contributors

See the LICENSE file for details.
