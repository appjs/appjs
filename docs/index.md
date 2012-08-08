### Basic Usage

    // get app object using the module system
    var app = require('appjs');

    // server files from the content directory relative to this .js file
    app.serveFilesFrom(__dirname + '/content');

    // app is the window factory. windows begin loading immediately
    var window = app.createWindow();

    // when ready is emitted the window has morphed into browser global window
    window.on('ready', function(){
      window.show();
    });

## App
__App__ is the entry point to AppJS. It is the access point for appjs generated interfaces, application lifecycle, events, and in-process serving of files and routes to the browser.

* __app.serveFilesFrom(path)__: Set path to serve static files from when the browser requests from __http://appjs/*__
* __app.createWindow(options)__: Creates a new hidden browser window with the given options and returns it.  This window will begin loading immediately if it has a url to go to.

#### Window Creation Options
Most of these options are adjustable after window creation. They are all optional with the defaults shown below.

* __width__           = 640
* __height__          = 460
* __left__            = centered
* __top__             = centered
* __name__            = auto-generated, it's only used to identify the window on `app.windows`
* __url__             = 'http://appjs' - serve static file root and routers
* __autoResize__      = false - resizes in response to html content
* __showChrome__      = true - show border and title bar
* __resizable__       = false - control if users can resize window
* __disableSecurity__ = true - allow cross origin requests
* __opacity__         = 1 - flat percent opacity for window
* __alpha__           = false - per-pixel alpha blended (Win & Mac)
* __fullscreen__      = false - client area covers whole screen
* __icons__           = {} - window icon in tray/taskbar/titlebar

## Window

Window objects are created and returned by app.createWindow. A window object will itself be the global object of the browser contect once the page has loaded. If the page loads a new url the same window object will morph to the uninitialized state and then become the new browser global when it is ready.

#### Window Events

* __window.on('create')__ - window has been created and is ready for the first time
* __window.on('ready')__ - window context is ready to use, including browser global
* __window.on('close')__ - window has been closed entirely and has ceased to exist
* __window.on('minimize')__ - window was just minimized
* __window.on('maximize')__ - window was just maximized
* __window.on('fullscreen')__ - window was just fullscreened
* __window.on('restore')__ - window was just restored
* __window.on('move')__ - window was just moveed
* __window.on('resize')__ - window was just resized

## Frame

Most of the controls to control the browser container are found on `window.frame`. window.frame is always available, whether the browser's global context is current initialized or not.

#### Frame Properties

* __window.frame.left__ - integer
* __window.frame.top__ - integer
* __window.frame.width__ - integer
* __window.frame.height__ - integer
* __window.frame.opacity__ - float between 0 and 1
* __window.frame.title__ - string
* __window.frame.state__ - one of ['normal', 'maximized', 'minimized', 'fullscreen']
* __window.frame.resizable__ - boolean
* __window.frame.autoResize__ - boolean
* __window.frame.showChrome__ - boolean
* __window.frame.alpha__ - boolean
* __window.frame.topmost__ - boolean

#### Frame Methods

* __window.frame.fullscreen()__
* __window.frame.minimize()__
* __window.frame.maximize()__
* __window.frame.restore()__
* __window.frame.center()__
* __window.frame.show()__
* __window.frame.hide()__
* __window.frame.resize(width, height)__
* __window.frame.move(left, top, [width], [height])__
* __window.frame.fade(toOpacity, milliseconds, callback)__
* __window.frame.drag()__ - (Windows only currently)
* __window.frame.openDevTools()__
* __window.frame.closeDevTools()__



## Debugging tips

On Windows plateform, you can launch your application in a console:

    bin\node.exe --harmony app.js

Do not forget you can open Chrome Console using a line of javascript :

    window.frame.openDevTools()
