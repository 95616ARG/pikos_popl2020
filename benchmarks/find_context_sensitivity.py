#!/usr/bin/env python3
import subprocess
from multiprocessing import Pool
import sys

TIMEOUT = 14400 # 4 hours
INITIALCS = 18
MAXCS = 50
NUMTHREADS = 16

def check_timeout(benchmark, cs):
    '''
    Runs the benchmark with given context-sensitivity (cs) using runexec.
    Return True if timed out, False otherwise.
    '''
    result = subprocess.run([
        'runexec', '--no-container', '--walltimelimit=%d' % TIMEOUT, '--output=/dev/null',
        '--',
        'pikos', '--rm-db', '-q', '-w', '--display-times=no', '--display-summary=no',
        '--progress=no', '--inline-all', '--proc=inter', '--no-checks', '--no-fixpoint-cache',
        '-cs=%d' % cs,
        '%s' % benchmark],
        stdout=subprocess.PIPE)
    res = result.stdout.decode('utf-8')
    if 'terminationreason=walltime' in res:
        return True
    if 'exitcode=0' not in res:
        raise Exception('error in ikos')
    return False

def binary_search_cs(benchmark):
    print('Searching cs for %s' % benchmark)
    found = False
    cs = INITIALCS
    step = 2
    upper_bound = None
    lower_bound = 1
    # Invariants:
    # - lower_bound <= cs < upper_bound
    # - Benchmark times out with cs greater than or equal to upper_bound.
    # - It doesn't time out with cs less than or equal to lower_bound.
    # upper_bound - lower_bound reduces on every iteration.
    # We want lower_bound + 1 == upper_bound in the end.
    while upper_bound is None or lower_bound + 1 != upper_bound:
        print('-- trying cs=%d for %s' % (cs, benchmark))
        try:
            if check_timeout(benchmark, cs):
                # Timed out
                upper_bound = cs
                cs = (lower_bound + cs) // 2
            else:
                # Didn't time out
                lower_bound = cs
                if upper_bound is None:
                    # Upper bound on cs not found yet.
                    if cs + step >= MAXCS:
                        return cs
                    cs = cs + step
                    step = 2 * step
                else:
                    # Upper bound already found.
                    cs = (cs + upper_bound) // 2
        except:
            return -1
    print("Found! %s, %d" % (benchmark, cs))
    return cs

if __name__ == '__main__':
    filename = '4hours.txt'
    numthreads = NUMTHREADS
    b_lst = []
    with open(filename, 'r') as f:
        for line in f:
            b_lst.append(line.strip())
    pool = Pool(numthreads)
    cs_lst = pool.map(binary_search_cs, b_lst)
    with open('context_sensitivity.csv', 'w') as f:
        for b, cs in zip(b_lst, cs_lst):
            f.write(str(b))
            f.write(',')
            f.write(str(cs))
            f.write('\n')
