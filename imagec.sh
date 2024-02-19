#!/bin/sh
appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname
export QT_QPA_PLATFORM=xcb
export QT_DEBUG_PLUGINS=1
export LD_LIBRARY_PATH
$dirname/$appname "$@"
