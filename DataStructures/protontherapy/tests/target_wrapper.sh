#!/bin/sh
LD_LIBRARY_PATH=/home/hofverberg/Qt5.7.0/5.7/gcc_64/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export LD_LIBRARY_PATH
QT_PLUGIN_PATH=/home/hofverberg/Qt5.7.0/5.7/gcc_64/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec "$@"
