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

    mpl.rcParams['text.usetex'] = True
    mpl.rcParams['text.latex.preamble'] = [r'\usepackage{amsmath}']
    mpl.rcParams['font.size'] = 17.0
    mpl.rcParams['axes.labelsize'] = 20.0
    mpl.rcParams['ytick.labelsize'] = 16.0
    mpl.rcParams['xtick.labelsize'] = 16.0

    df = df.sort_values(by='slope', ascending=False)
    x = np.arange(0, 1, 0.01)
    y = []
    for i in x:
        t = (df[df['slope'] >= i]).shape[0]
        y.append(t)
        if (i in [0.4]):
          plt.annotate("(%.1f, %d)" % (i, t), xy=(i, t),
                       xytext=(i+0.2, t+12),
                       textcoords="data",
                       arrowprops=dict(facecolor='black', shrink=0.05),
                       horizontalalignment='right',
                       verticalalignment='bottom')
    plt.plot(x, y, 'k')

    plt.xlim(0.0, 1.0)
    plt.xlabel('Scalability coefficient')
    plt.ylabel('Number of benchmarks')
    plt.savefig('fig9-b.png', bbox_inches='tight')
