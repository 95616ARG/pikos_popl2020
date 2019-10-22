#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")

cd benchmarks
tar xf benchmarks.tar.gz
