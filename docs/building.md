# Building
For all platforms [Node.js](http://nodejs.org/#download) must be installed. `node-gyp` is used to automate the build process and can be installed using npm `npm install node-gyp -g` (using sudo on Linux and Mac). When you have fulfilled the pre-requisites to build on your platform, AppJS can be built by executing `node-gyp rebuild` in the AppJS source folder.

The [Chromium Embedded Frame](https://github.com/appjs/appjs/downloads) distributables are required to build. Download the ones for your platform, extract the folder to `<appjs>/deps` and rename the folder to "cef".

## Linux

## Mac

`touch bundle-mac.sh` 
`chmod +x bundle-mac.sh`

Then paste the following in the `bundle-mac.sh` file:

```sh

  #!/usr/bin/env sh

  git clone git@github.com:appjs/appjs.git
  export NVERSION="0.8.22";
  export CEFVERSION="1.1180.724";
  export N32BASE=`pwd`/appjs/node-32/${NVERSION}
  export PATH="${N32BASE}/bin:${PATH}"
  export NODE_URL="http://nodejs.org/dist/v${NVERSION}/node-v${NVERSION}-darwin-x86.tar.gz"
  export NODE_TAR_FILE="node-v${NVERSION}-darwin-x86.tar.gz"
  export CEF_BINARY="cef_binary_${CEFVERSION}_darwin_ia32.tar.gz"
  export CEF_BINARY_URL="https://github.com/downloads/appjs/appjs/cef_binary_${CEFVERSION}_darwin_ia32.tar.gz"

  cd appjs
  mkdir -p node-32/src

  cd node-32/src

  if [ -a "${NODE_TAR_FILE}" ]; 
  then
    echo "\nFile ${NODE_TAR_FILE} exists.\n"
    rm -rf "node-v${NVERSION}-darwin-x86" # if it exists
  else  
    wget "${NODE_URL}"
  fi

  tar -zxf $NODE_TAR_FILE

  cd node-v${NVERSION}-darwin-x86
  export PATH="node-v${NVERSION}-darwin-x86/bin:${PATH}"
  echo "\nCopying node binary...\n"
  cp bin/node ../../../data/mac/node-bin/
  npm install -g node-gyp
  # mkdir node_modules
  npm install mime

  cd ../../../
  mkdir -p deps
  cd deps

  if [ -a "cef_binary_${CEFVERSION}_darwin_ia32.tar.gz" ]; 
  then
    echo "\nFile cef_binary_${CEFVERSION}_darwin_ia32.tar.gz exists.\n"
  else  
    wget --no-check-certificate "${CEF_BINARY_URL}"
  fi

  tar -xzf cef_binary_${CEFVERSION}_darwin_ia32.tar.gz
  ln -s cef_binary_${CEFVERSION}_darwin_ia32 cef

  cd ../
  node-gyp configure
  node-gyp build
  
```

Then run it:

`./bundle-mac.sh`

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
	
#### Building Appjs

* git clone https://github.com/appjs/appjs
  * If you are new to git you can try tortoise git http://code.google.com/p/tortoisegit/wiki/SetupHowTo
* Download the cef binary from https://github.com/appjs/appjs/downloads
  * extract to appjs/deps/cef
  * you can download the 32 bit and 64 bit version and swap between them when compiling for different architectures.
  * SET npm_config_arch=ia32 to build for 32 bit
  * SET npm_config_arch=x64 to build for 64 bit.
* In the "Windows SDK 7.1 Command Prompt"
  * cd to appjs directory
  * install node modules:
      * npm install mime
  * to build for 32bit on a 64bit machine:
      * setenv /x86
	  * SET npm_config_arch=ia32
  * then in the appjs directory type
      
	  node-gyp rebuild > build.log

#### Recommendations for building in windows

Use [node version 0.8.0](http://nodejs.org/dist/v0.8.0/node-v0.8.0-x86.msi)

Follow the following instructions in the *specified order* :

If you already have Microsoft Visual Studio C++ 2010 or SP1 or Windows 7 SDK [this means any of them] before trying to build appjs then **Uninstall** the following from the control panel:
 
	1. Microsoft Visual Studio 2010 Service Pack 1
	2. “Microsoft Visual C++ 2010 x86 Redistributable” and "Microsoft Visual C++ 2010 x64 Redistributable"
	3. Microsoft Visual Studio C++ 2010
	4. Windows SDK 7.1 and its Service Pack 1

Then **install** in the *specified order* only:

	1. Microsoft Visual Studio C++ 2010 
	2. Windows SDK 7.1
	3. Visual Studio 2010 Service Pack 1
	4. Visual C++ 2010 SP1 Compiler Update for the Windows SDK 7.1

##### Build Errors
If build fails with `Linker Errors with native_menu.obj`

Add `/FORCE:MULTIPLE` as an additional option in the Linker Properties of *binding.gyp*, like:
        
    'VCLinkerTool': {
	'LinkTimeCodeGeneration': 1, # link-time code generation
	'OptimizeReferences': 2, # /OPT:REF
	'AdditionalOptions': "/FORCE:MULTIPLE",   # <- add this line
	'EnableCOMDATFolding': 2, # /OPT:ICF
	'LinkIncremental': 1, # disable incremental linking
	},	  
 

