#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")

cd benchmarks
wget https://pikos-popl2020.s3-us-west-2.amazonaws.com/benchmarks.tar.gz
tar xf benchmarks.tar.gz
