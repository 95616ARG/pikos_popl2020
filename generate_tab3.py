#!/usr/bin/env python3.6
import subprocess
import time
import os
import glob
import pandas as pd
import datetime
import math
import matplotlib.pyplot as plt
import sys
import numpy as np
import scipy.stats
import scipy.stats.mstats

if __name__ == '__main__':
    df = pd.read_csv(sys.argv[1])
    name = 'benchmark'
    cat = 'category'
    base = 'walltime (s)'
    ours = 'walltime (s)-4'
    speedup = 'speedup-4'
    df = df[df[base] >= 5]

    df = df.sort_values(by=speedup, ascending=False)
    dfs = df[0:5]
    dfs = dfs[[name, cat, base, ours, speedup]]
    dfs = dfs.round(2)
    dfs.to_csv('tab3-speedup.csv', index=False)
    print(dfs)

    df = df.sort_values(by=base, ascending=False)
    dfs = df[0:5]
    dfs = dfs[[name, cat, base, ours, speedup]]
    dfs = dfs.round(2)
    dfs.to_csv('tab3-ikos.csv', index=False)
    print(dfs)
