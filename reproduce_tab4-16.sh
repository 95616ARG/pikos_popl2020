#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
export LD_PRELOAD="/usr/local/lib/libtcmalloc.so"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
export PATH="$SCRIPT_DIR/build/run/bin:$PATH"
export PYTHONPATH="$SCRIPT_DIR:$PYTHONPATH"

benchexec --container ./xml/tab4.xml
table-generator results/*.xml.bz2 -o . -f csv -n tmp
tail -n +4 tmp.table.csv > tmp.csv
sed -i '1 i\benchmark,status,cputime (s),walltime (s),memUsage,status-4,cputime (s)-4,walltime (s)-4,memUsage-4,status-8,cputime (s)-8,walltime (s)-8,memUsage-8,status-12,cputime (s)-12,walltime (s)-12,memUsage-12,status-16,cputime (s)-16,walltime (s)-16,memUsage-16' tmp.csv
sed -i 's/\t/,/g' tmp.csv
$SCRIPT_DIR/update_speedup.py 16 tab4
rm tmp.table.csv
rm -rf results
