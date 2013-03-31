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
