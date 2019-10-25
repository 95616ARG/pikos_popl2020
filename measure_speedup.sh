#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
export LD_PRELOAD="/usr/local/lib/libtcmalloc.so"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
export PATH="$SCRIPT_DIR/build/run/bin:$PATH"

read IKOS_EC IKOS_T <<< $(runexec --quiet --container --walltimelimit=14400 --output=/dev/null -- ./run_ikos.sh --rm-db -q -w --display-times=no --display-summary=no --progress=no --inline-all --no-checks --no-fixpoint-cache --proc=inter $@ \
  | awk -F= '/exitcode|walltime/ {print $2}')

if [[ $IKOS_EC != 0 ]]
then
  echo "IKOS failed with exitcode $IKOS_EC"
  exit $IKOS_EC
fi

read PIKOS_EC PIKOS_T <<< $(runexec --quiet --container --walltimelimit=14400 --output=/dev/null -- ./run_pikos.sh --rm-db -q -w --display-times=no --display-summary=no --progress=no --inline-all --no-checks --no-fixpoint-cache --proc=inter $@ \
  | awk -F= '/exitcode|walltime/ {print $2}')

if [[ $PIKOS_EC != 0 ]]
then
  echo "PIKOS failed with exitcode $PIKOS_EC"
  exit $PIKOS_EC
fi

IKOS_T=${IKOS_T%?}
PIKOS_T=${PIKOS_T%?}
SPEEDUP=$(bc -l <<< "$IKOS_T/$PIKOS_T")
printf "Running time of IKOS  = %.5f seconds.\n" $IKOS_T
printf "Running time of PIKOS = %.5f seconds.\n" $PIKOS_T
printf "Speedup (running time of IKOS / running time of PIKOS) = %.2fx.\n" $SPEEDUP
