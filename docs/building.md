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
    tar -xzf cef_binary_${CEFVERSION}_darwin_ia32.tar.gz
    ln -s cef_binary_${CEFVERSION}_darwin_ia32 cef
    cd ..
    node-gyp build

## Windows
Visual C++ 2010 must be available in some form. You can download [Visual C++ 2010 Express](http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-cpp-express) for free.

While not required, [MozillaBuild](http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-Latest.exe) is the recommended build environment to compile in. It ensures you have all the dependencies and environmental variables in place. When you want to compile simply run "start-msvc10.bat" in the MozillaBuild folder, then `cd /c/path/to/appjs`.

### Windows Alternative (detailed)

This section explains how to take a fresh windows 7 install and gives detailed instructions on how to compile appjs.

#### NPM / node-gyp
The first step involves creating a working npm environment that is able to compile C++ modules.
* Go to [nodejs.org](http://nodejs.org/) and click on the install button to get the latest version of node.
* Click on the "Node.js Command Prompt" link and then type in:

      npm install -g node-gyp 
* Install python (version 2.7.3 since version 3.* versions are not compatible).
  * http://www.python.org/download/releases/2.7.3/
* Create an environment variable PYTHON = <install directory>\python.exe

#### Free C++ Compiler setup
This section details using the free visual studio express edition since that is available for all. 

* Download the Microsoft Visual Studio C++ 2010 Express (http://go.microsoft.com/?linkid=9709949)
 * This link is a web installer and it installs a lot of stuff (2.3 gig)
 * install includes .Net Framework 4 and Microsoft SQL Server Compact

* For windows 7 you will also need to download and install Microsoft Windows SDK for Windows 7
  * Link is another web installer that installs a further 1.6 gig
  * http://www.microsoft.com/en-us/download/details.aspx?displayLang=en&id=8279

* Service Pack 1 can then be installed
  * http://www.microsoft.com/downloads/en/confirmation.aspx?FamilyID=75568aa6-8107-475d-948a-ef22627e57a5
  * VC-Compiler-KB2519277.exe (http://www.microsoft.com/en-us/download/details.aspx?id=4422)

To test for a working environment you can open the nodejs command prompt and enter the following:

     npm install sqlite3

It may give some warnings but as long as it does not show any red colours then it should print something like
    
	sqlite3@2.1.5 node_modules\sqlite3
	
