#!/usr/bin/env python3
import subprocess
import time
import os
import glob
import pandas as pd
import datetime
import math
import matplotlib.pyplot as plt
import matplotlib as mpl
import sys
import numpy as np
import scipy.stats
import scipy.stats.mstats

if __name__ == '__main__':
    df = pd.read_csv('tmp.csv')
    base = 'walltime (s)'
    df.drop([0], inplace=True)
    c = int(sys.argv[1])
    for i, r in df.iterrows():
        if r['benchmark'].startswith('OSS'):
            df.at[i, 'category'] = 'OSS'
        elif r['benchmark'].startswith('SVC'):
            df.at[i, 'category'] = 'SVC'
        if c == 8:
            df.at[i, 'speedup-2'] = r[base] / r[base+'-2']
            df.at[i, 'speedup-4'] = r[base] / r[base+'-4']
            df.at[i, 'speedup-6'] = r[base] / r[base+'-6']
            df.at[i, 'speedup-8'] = r[base] / r[base+'-8']
        elif c == 4:
            df.at[i, 'speedup-4'] = r[base] / r[base+'-4']
        elif c == 16:
            df.at[i, 'speedup-4'] = r[base] / r[base+'-4']
            df.at[i, 'speedup-8'] = r[base] / r[base+'-8']
            df.at[i, 'speedup-12'] = r[base] / r[base+'-12']
            df.at[i, 'speedup-16'] = r[base] / r[base+'-16']
    df.to_csv(sys.argv[2] + '.csv', index=False)
