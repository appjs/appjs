#!/bin/bash

# set necessary variables
APPJS_BIN_DIR=`dirname $(readlink -f ${0})`
APPJS_ROOT_DIR=`dirname $APPJS_BIN_DIR`
VERSION="latest"
USE_SYSTEM_NODE=0

# backup arguments for later use
args="$@"

set -- `getopt :nv: "$@"`

# detect appjs version to use
while [ $# -gt 0 ]
do
	case "$1" in
		-v)		VERSION="$2"; shift; shift;;
		-n)		USE_SYSTEM_NODE=1; shift;;
		--)		shift; break;;
		*)		break;;
	esac
done

# set proper appjs module to use according to the selected version
APPJS_MODULE_PATH="$APPJS_ROOT_DIR/dists/$VERSION/"
APPJS_MODULE_BIN_PATH="$APPJS_MODULE_PATH/node_modules/appjs/bin/appjs"

# set NODE_PATH to include appjs module
NODE_PATH="$APPJS_MODULE_PATH:$NODE_PATH"

if [ "$USE_SYSTEM_NODE" = "0" ]
then
# devs may not have node installed
PATH="$APPJS_BIN_DIR:$PATH"
fi

# run appjs sdk tool with original arguments
$APPJS_MODULE_BIN_PATH $args
