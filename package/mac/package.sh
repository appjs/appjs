#!/bin/bash

function realpath_f () {(cd "$1" && echo "$(pwd -P)")}

basedir="$( realpath_f `dirname ${0}` )";
basedir="$( dirname `dirname ${basedir}` )";

MACDIR="${basedir}/package/mac";
TARDIR="${MACDIR}/dist"

if [ -d ${TARDIR} ]; then
  echo "Error: ${TARDIR} exists.";
  exit 1;
fi;

mkdir -p ${TARDIR};
cp -Rpa ${MACDIR}/AppBundle.skel.app ${TARDIR}/AppJS_Example.app;
cp -Rpa ${basedir}/app/data/bin/* ${TARDIR}/AppJS_Example.app/Contents/MacOS/bin/;
cp -Rpa ${basedir}/app/data/node_modules ${TARDIR}/AppJS_Example.app/Contents/MacOS/;
cp -Rpa ${basedir}/app/data/content ${TARDIR}/AppJS_Example.app/Contents/Resources/;
cp -Rpa ${basedir}/app/data/app.js ${TARDIR}/AppJS_Example.app/Contents/Resources/;
open ${TARDIR}/AppJS_Example.app;
