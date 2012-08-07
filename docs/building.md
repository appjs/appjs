# Building
For all platforms [Node.js](http://nodejs.org/#download) must be installed. `node-gyp` is used to automate the build process and can be installed using npm `npm install node-gyp -g` (using sudo on Linux and Mac). When you have fulfilled the pre-requisites to build on your platform, AppJS can be built by executing `node-gyp rebuild` in the AppJS source folder.

The [Chromium Embedded Frame](https://github.com/appjs/appjs/downloads) distributables are required to build. Download the ones for your platform, extract the folder to `<appjs>/deps` and rename the folder to "cef".

## Linux

## Mac

## Windows
Visual C++ 2010 must be available in some form. You can download [Visual C++ 2010 Express](http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-cpp-express) for free.

While not required, [MozillaBuild](http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-Latest.exe) is the recommended build environment to compile in. It ensures you have all the dependencies and environmental variables in place. When you want to compile simply run "start-msvc10.bat" in the MozillaBuild folder, then `cd /c/path/to/appjs`.
