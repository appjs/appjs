# Building
For all platforms [Node.js](http://nodejs.org/#download) must be installed. `node-gyp` is used to automate the build process and can be installed using npm `npm install node-gyp -g` (using sudo on Linux and Mac). When you have fulfilled the pre-requisites to build on your platform, AppJS can be built by executing `node-gyp rebuild` in the AppJS source folder.

The [Chromium Embedded Frame](https://github.com/appjs/appjs/downloads) distributables are required to build. Download the ones for your platform, extract the folder to `<appjs>/deps` and rename the folder to "cef".

## Linux

## Mac

    git clone git@github.com:appjs/appjs.git
    export NVERSION="0.8.5";
    export CEFVERSION="1.1180.724";
    export N32BASE=`pwd`/appjs/node-32/${NVERSION}
    cd appjs
    mkdir -p node-32/${NVERSION} node-32/src
    cd node-32/src
    wget http://nodejs.org/dist/v${NVERSION}/node-v${NVERSION}.tar.gz
    tar -xzf node-v${NVERSION}.tar.gz
    cd node-v${NVERSION}
    ./configure --prefix=${N32BASE} --dest-cpu=ia32
    make
    make install
    cd ../../../
    export PATH="${N32BASE}/bin:${PATH}"
    npm install -g node-gyp
    mkdir node_modules
    npm install mime
    mkdir deps
    cd deps
    wget --no-check-certificate https://github.com/downloads/appjs/appjs/cef_binary_${CEFVERSION}_darwin_ia32.tar.gz
    tar -xzf https://github.com/downloads/appjs/appjs/cef_binary_${CEFVERSION}_darwin_ia32.tar.gz
    ln -s cef_binary_${CEFVERSION}_darwin_ia32 cef
    cd ..
    node-gyp build

## Windows
Visual C++ 2010 must be available in some form. You can download [Visual C++ 2010 Express](http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-cpp-express) for free.

While not required, [MozillaBuild](http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-Latest.exe) is the recommended build environment to compile in. It ensures you have all the dependencies and environmental variables in place. When you want to compile simply run "start-msvc10.bat" in the MozillaBuild folder, then `cd /c/path/to/appjs`.
