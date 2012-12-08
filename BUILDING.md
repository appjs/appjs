## To Build

### Mac

These instructions work on 10.6

clone the repository

    cd appjs

    npm install mime

Download `node-v0.8.15-darwin-x86.tar.gz`

extract and grab the `bin/node` executable and stick it in `data/mac/node-bin/`

    node-gyp rebuild --arch i386 -j 4 # the space matters after the -j
