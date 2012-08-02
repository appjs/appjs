# AppJS
AppJS is an SDK to develop desktop applications using Node.js melded with Chromium. With AppJS you can develop desktop tools and applications using the same libraries and knowledge used to build websites. You get all the following in one package:

* JS, HTML5, CSS, SVG, WebGL provided by Chromium
* mature http/https servers and client APIs - Node
* filesystem, dns, cryptography, subprocesses, OS APIs - Node
* sandboxed code execution environements virtual machines - Node
* tools for exposing native C++ bindings to JavaScript--APIs for authoring and the tools for compiling - Node

---

* __Site: [appjs.org](http://appjs.org)__
* __[Google+](https://plus.google.com/u/0/112803615725562877790/posts)__
* __Mailing List: [mailing list](https://groups.google.com/forum/#!forum/appjs-dev)__
* __IRC: #appjs at irc.freenode.net__

---

__AppJS is under heavy development. Expect many API changes and things to break.__

## 30 Second Quickstart
The below packages include everything needed to get started with AppJS, including Node.js, all dependencies, binaries, and a launcher ready to go out of the box. 1.) Extract to a folder. 2.) Double click on launch. 3.) Hello World.

__AppJS 0.0.17 Distributables:__

* Linux [32 bit](http://dists.appjs.org/0.0.17/appjs-0.0.17-linux-ia32.tar.gz) / [64 bit](http://dists.appjs.org/0.0.17/appjs-0.0.17-linux-x64.tar.gz) -> app.sh
* [Mac](http://dists.appjs.org/0.0.17/appjs-0.0.17-darwin-ia32.zip) -> app.sh
* [Windows](http://dists.appjs.org/0.0.17/appjs-0.0.17-win32-ia32.zip) -> app.exe

(Windows requires [MSVC++ 2010 runtimes](http://www.microsoft.com/en-us/download/details.aspx?id=5555))

## One Minute Usage Overview
```javascript
var app = module.exports = require('appjs');

app.serveFilesFrom(__dirname + '/content');

var window = app.createWindow({
  width  : 640,
  height : 460,
  icons  : __dirname + '/content/icons'
});

window.on('create', function(){
  console.log("Window Created");
  window.frame.show();
  window.frame.center();
});

window.on('ready', function(){
  console.log("Window Ready");
  window.require = require;
  window.process = process;
  window.module = module;
  window.addEventListener('keydown', function(e){
    if (e.keyIdentifier === 'F12') {
      window.frame.openDevTools();
    }
  });
});

window.on('close', function(){
  console.log("Window Closed");
});
```

## Shared Contexts

![chrome-in-node-repl](https://github.com/appjs/appjs/raw/master/examples/chrome-in-node-repl.jpg "Chrome in Node REPL")

![DOM Event Listener in Node](https://github.com/appjs/appjs/raw/master/examples/shared-context.jpg "DOM Event Listener in Node")



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


## License
( The MIT License )

Copyright (c) 2012 Morteza Milani and other AppJS contributors

See the LICENSE file for details.
