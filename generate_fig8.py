#!/usr/bin/env python3.6
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
    mpl.rcParams['text.usetex'] = True
    mpl.rcParams['text.latex.preamble'] = [r'\usepackage{amsmath}']
    mpl.rcParams['font.size'] = 17.0
    mpl.rcParams['axes.labelsize'] = 20.0
    mpl.rcParams['ytick.labelsize'] = 22.0
    mpl.rcParams['xtick.labelsize'] = 22.0

    df = pd.read_csv(sys.argv[1])
    df = df[df['walltime (s)'] >= 5]
    slopes = []
    for i, r in df.iterrows():
        slope, _, _, _, _ = scipy.stats.linregress([2,4,6,8], r[['speedup-2', 'speedup-4', 'speedup-6' ,'speedup-8']].astype(float))
        slopes.append(slope)
        df.at[i, 'slope'] = slope

    # df.to_csv('base-nt2-nt4-nt6-nt8-slope.csv',index=False)
    df = df[df['slope'] >= 0.0]
    df = df.sort_values(by='slope')
    nr, nc = df.shape
    
    tmp = 3*(nr//4)
    lst = (df.tail(1).append(df[tmp:tmp+1])).append(df[0:1])
    marker = ['+', 'x', '.']
    cnt = 0
    print('Benchmarks used in fig9-a:')
    for i, r in lst.iterrows():
        print(' - ', r['benchmark'])
        x = np.array([2,4,6,8])
        slope, intercept, _,_,_ =scipy.stats.linregress([2,4,6,8], r[['speedup-2', 'speedup-4', 'speedup-6' ,'speedup-8']].astype(float))
        plt.scatter([2,4,6,8], r[['speedup-2','speedup-4', 'speedup-6' ,'speedup-8']], label='Scalability coefficient = %.2f' % round(slope,2), marker=marker[cnt])
        cnt+=1
        plt.plot(x, intercept + slope * x)

    plt.legend(loc='upper left')
    plt.ylim(0,8)
    plt.yticks([0,1,2,4,6,8])
    plt.xlabel(r'Number of threads $k$')
    plt.ylabel(r'Speedup of \textsc{Pikos}\textlangle{}$k$\textrangle{}')
    plt.savefig('fig8-a.png', bbox_inches='tight')

    plt.clf()
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
    plt.ylim(0,1000)
    plt.xlabel('Scalability coefficient')
    plt.ylabel('Number of benchmarks')
    plt.savefig('fig8-b.png', bbox_inches='tight')
    print('\nFigures saved to "fig8-a.png" and "fig8-b.png".')
