#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
export LD_PRELOAD="/usr/local/lib/libtcmalloc.so"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
export PATH="$SCRIPT_DIR/build/run/bin:$PATH"
export PYTHONPATH="$SCRIPT_DIR:$PYTHONPATH"

RES=results$(date +%m-%d-%H-%M)/
benchexec --no-container -o $RES ./xml/tab2a.xml
table-generator $RES/*base.xml.bz2 $RES/*cc4.xml.bz2 -o . -f csv -n tmp
tail -n +4 tmp.table.csv > tmp.csv
sed -i '1 i\benchmark,status,cputime (s),walltime (s),memUsage,status-4,cputime (s)-4,walltime (s)-4,memUsage-4' tmp.csv
sed -i 's/\t/,/g' tmp.csv
$SCRIPT_DIR/update_speedup.py 4 tab2a
rm tmp.table.csv
