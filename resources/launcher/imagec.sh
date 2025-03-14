#!/bin/sh
appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

QT_QPA_PLATFORM_PLUGIN_PATH=$dirname/plugins/platforms
export QT_QPA_PLATFORM_PLUGIN_PATH

#offscreen
QT_QPA_PLATFORM="xcb"
export QT_QPA_PLATFORM

LD_LIBRARY_PATH=$dirname/lib
export LD_LIBRARY_PATH

$dirname/$appname "$@"
