#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
export LD_PRELOAD="/usr/local/lib/libtcmalloc.so"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
export PATH="$SCRIPT_DIR/build/run/bin:$PATH"

echo Running IKOS...
read IKOS_EC IKOS_T <<< $(runexec --quiet --no-container --walltimelimit=14400 --output=/dev/null -- ./run_ikos.sh --rm-db -q -w --display-times=no --display-summary=no --progress=no --inline-all --no-checks --no-fixpoint-cache --proc=inter $SCRIPT_DIR/benchmarks/OSS/audit-2.8.4/aureport.bc \
  | awk -F= '/exitcode|walltime/ {print $2}')

if [[ $IKOS_EC != 0 ]]
then
  echo "IKOS failed with exitcode $IKOS_EC"
  exit $IKOS_EC
fi

echo Running PIKOS\<4\>...
read PIKOS_EC4 PIKOS_T4 <<< $(runexec --quiet --no-container --walltimelimit=14400 --output=/dev/null -- ./run_pikos.sh --rm-db -q -w --display-times=no --display-summary=no --progress=no --inline-all --no-checks --no-fixpoint-cache --proc=inter -nt=4 $SCRIPT_DIR/benchmarks/OSS/audit-2.8.4/aureport.bc \
  | awk -F= '/exitcode|walltime/ {print $2}')

if [[ $PIKOS_EC4 != 0 ]]
then
  echo "PIKOS failed with exitcode $PIKOS_EC4"
  exit $PIKOS_EC4
fi

echo Running PIKOS\<8\>...
read PIKOS_EC8 PIKOS_T8 <<< $(runexec --quiet --no-container --walltimelimit=14400 --output=/dev/null -- ./run_pikos.sh --rm-db -q -w --display-times=no --display-summary=no --progress=no --inline-all --no-checks --no-fixpoint-cache --proc=inter -nt=8 $SCRIPT_DIR/benchmarks/OSS/audit-2.8.4/aureport.bc \
  | awk -F= '/exitcode|walltime/ {print $2}')

if [[ $PIKOS_EC8 != 0 ]]
then
  echo "PIKOS failed with exitcode $PIKOS_EC8"
  exit $PIKOS_EC8
fi

echo Running PIKOS\<12\>...
read PIKOS_EC12 PIKOS_T12 <<< $(runexec --quiet --no-container --walltimelimit=14400 --output=/dev/null -- ./run_pikos.sh --rm-db -q -w --display-times=no --display-summary=no --progress=no --inline-all --no-checks --no-fixpoint-cache --proc=inter -nt=12 $SCRIPT_DIR/benchmarks/OSS/audit-2.8.4/aureport.bc \
  | awk -F= '/exitcode|walltime/ {print $2}')

if [[ $PIKOS_EC12 != 0 ]]
then
  echo "PIKOS failed with exitcode $PIKOS_EC12"
  exit $PIKOS_EC12
fi

echo Running PIKOS\<16\>...
read PIKOS_EC16 PIKOS_T16 <<< $(runexec --quiet --no-container --walltimelimit=14400 --output=/dev/null -- ./run_pikos.sh --rm-db -q -w --display-times=no --display-summary=no --progress=no --inline-all --no-checks --no-fixpoint-cache --proc=inter -nt=16 $SCRIPT_DIR/benchmarks/OSS/audit-2.8.4/aureport.bc \
  | awk -F= '/exitcode|walltime/ {print $2}')

if [[ $PIKOS_EC16 != 0 ]]
then
  echo "PIKOS failed with exitcode $PIKOS_EC16"
  exit $PIKOS_EC16
fi

IKOS_T=${IKOS_T%?}
PIKOS_T4=${PIKOS_T4%?}
PIKOS_T8=${PIKOS_T8%?}
PIKOS_T12=${PIKOS_T12%?}
PIKOS_T16=${PIKOS_T16%?}
SPEEDUP4=$(bc -l <<< "$IKOS_T/$PIKOS_T4")
SPEEDUP8=$(bc -l <<< "$IKOS_T/$PIKOS_T8")
SPEEDUP12=$(bc -l <<< "$IKOS_T/$PIKOS_T12")
SPEEDUP16=$(bc -l <<< "$IKOS_T/$PIKOS_T16")
printf "Running time of IKOS  = %.5f seconds.\n" $IKOS_T
printf "Running time of PIKOS<4> = %.5f seconds.\n" $PIKOS_T4
printf "Speedup (running time of IKOS / running time of PIKOS<4>) = %.2fx.\n" $SPEEDUP4
printf "Running time of PIKOS<8> = %.5f seconds.\n" $PIKOS_T8
printf "Speedup (running time of IKOS / running time of PIKOS<8>) = %.2fx.\n" $SPEEDUP8
printf "Running time of PIKOS<12> = %.5f seconds.\n" $PIKOS_T12
printf "Speedup (running time of IKOS / running time of PIKOS<12>) = %.2fx.\n" $SPEEDUP12
printf "Running time of PIKOS<16> = %.5f seconds.\n" $PIKOS_T16
printf "Speedup (running time of IKOS / running time of PIKOS<16>) = %.2fx.\n" $SPEEDUP16
