#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
export LD_PRELOAD="/usr/local/lib/libtcmalloc.so"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
export PYTHONPATH="$SCRIPT_DIR:$PYTHONPATH"
export PATH="$SCRIPT_DIR/build/run/bin:$PATH"

echo $PYTHONPATH
benchexec --no-container -o results $@
