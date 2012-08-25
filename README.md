# AppJS
AppJS is an SDK to develop desktop applications using Node.js melded with Chromium. With AppJS you can develop desktop tools and applications using the same libraries and knowledge used to build websites. You get all the following in one package:

* JS, HTML5, CSS, SVG, WebGL provided by Chromium
* mature http/https servers and client APIs - Node
* filesystem, dns, cryptography, subprocesses, OS APIs - Node
* sandboxed code execution environements virtual machines - Node
* tools for exposing native C++ bindings to JavaScript- Node

---

* __Site: [appjs.org](http://appjs.org)__
* __[Google+](https://plus.google.com/u/0/112803615725562877790/posts)__
* __Mailing List: [mailing list](https://groups.google.com/forum/#!forum/appjs-dev)__
* __IRC: #appjs at irc.freenode.net__

## 30 Second Quickstart
The below packages include everything needed to get started with AppJS, including Node.js, all dependencies, binaries, and a launcher ready to go out of the box. 1.) Extract to a folder. 2.) Double click on launch. 3.) Hello World.

__AppJS 0.0.19 Distributables:__

* Linux [32 bit](http://dists.appjs.org/0.0.19/appjs-0.0.19-linux-ia32.tar.gz) / [64 bit](http://dists.appjs.org/0.0.19/appjs-0.0.19-linux-x64.tar.gz) -> app.sh
* [Mac](http://dists.appjs.org/0.0.19/appjs-0.0.19-darwin-ia32.zip) -> app.sh
* [Windows](http://dists.appjs.org/0.0.19/appjs-0.0.19-win32-ia32.zip) -> app.exe

__npm install__
*OS X install is currently broken*

AppJS can be now be installed via npm.

    npm install appjs

__AppJS requires 32bit Node on OS X__. It works on 64bit OS X but __Node must be 32bit__. We're working on solving this, but it's a limitation of Chrome itself so it's a work in progress.
Help us gain traction by [starring this chromium issue](http://code.google.com/p/chromium/issues/detail?id=18323).


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



## License
The MIT License

Copyright (c) 2012 The AppJS Authors

See the LICENSE file for details.
