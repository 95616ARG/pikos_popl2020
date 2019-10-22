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
    df = pd.read_csv(sys.argv[1])
    slopes = []
    for i, r in df.iterrows():
        slope, _, _, _, _ = scipy.stats.linregress([4,6,8], r[['speedup-4', 'speedup-6' ,'speedup-8']].astype(float))
        slopes.append(slope)
        df.at[i, 'slope'] = slope
    # df.to_csv('base-nt2-nt4-nt6-nt8-slope.csv',index=False)

    df = df.sort_values(by='slope', ascending=False)
    df[0:5][['benchmark', 'category', 'walltime (s)', 'speedup-4', 'speedup-6', 'speedup-8', 'slope']].to_csv('tab4-candidates.csv', index=False)
