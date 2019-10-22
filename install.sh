#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
cd $SCRIPT_DIR

# Build ikos
rm -rf build && mkdir build
cd build
export MAKEFLAGS="-j8"
cmake \
    -DCMAKE_INSTALL_PREFIX="run" \
    -DLLVM_CONFIG_EXECUTABLE="/usr/lib/llvm-8/bin/llvm-config" \
    ../pikos
make
make install
