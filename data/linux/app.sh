#!/bin/sh

DIR="$( cd "$( dirname "$0" )" && pwd )"
$DIR/data/bin/node --harmony $DIR/data/app.js $1
