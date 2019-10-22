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
    mpl.rcParams['text.usetex'] = True
    mpl.rcParams['text.latex.preamble'] = [r'\usepackage{amsmath}']
    mpl.rcParams['font.size'] = 17.0
    mpl.rcParams['axes.labelsize'] = 20.0
    mpl.rcParams['ytick.labelsize'] = 20.0
    mpl.rcParams['xtick.labelsize'] = 20.0
    mpl.rcParams['font.weight'] = 'bold'

    df = pd.read_csv(sys.argv[1])
    base = 'walltime (s)'
    ours = 'walltime (s)-4'
    speedup = 'speedup-4'
    df = df[df[base] >= 5]

    ax = plt.subplot(111)
    ax.set_xscale('log')
    ax.set_yscale('log')

    plt.scatter(df[base].values, df[ours].values, marker='.', alpha=0.8)

    plt.rc('text', usetex=True)
    plt.grid(b=True, ls='--', alpha=0.3)
    plt.xlabel(r'Analysis time of $\textsc{Ikos}$ (seconds)')
    plt.ylabel(r'Analysis time of $\textsc{Pikos\textlangle4\textrangle}$ (seconds)')

    mins = df.loc[df[speedup].idxmin()]
    maxs = df.loc[df[speedup].idxmax()]
    plt.annotate("Minimum speedup = %.2fx" % round(mins[speedup], 2),
            xy=(mins[base],mins[ours]),
            xytext=(0.55, 0.6),
            textcoords='axes fraction', arrowprops=dict(facecolor='black', shrink=0.05), horizontalalignment='right', verticalalignment='bottom')

    plt.annotate("Maximum speedup = %.2fx" % round(maxs[speedup], 2),
            xy=(maxs[base],maxs[ours]),
            xytext=(0.98, 0.08),
            textcoords='axes fraction', arrowprops=dict(facecolor='black', shrink=0.05), horizontalalignment='right', verticalalignment='bottom')

    mx = max(df[base].max(), df[ours].max()) + 1000
    mn = 1

    plt.xlim(mn, mx)
    plt.ylim(mn, mx)

    lims = [mn,mx]
    lims2 = [mn/2.0,mx/2.0]
    lims4 = [mn/4.0,mx/4.0]

    ax.plot(lims, lims, 'k-', alpha=1, zorder=0, label="1.00x speedup")
    ax.plot(lims, lims2, 'k-.', alpha=1, zorder=0, label="2.00x speedup")
    ax.plot(lims, lims4, 'k:', alpha=1, zorder=0, label="4.00x speedup")

    plt.legend()

    phm = scipy.stats.hmean(df[speedup].values)
    pgm = scipy.stats.mstats.gmean(df[speedup].values)
    print('Harmonic mean: ', phm)
    print('Geometric mean: ', pgm)
    print('Arithmetic mean: ', df[speedup].mean())

    r, c = df.shape
    df = df.sort_values(by=base, ascending=False)[0:r//4]
    phmq = scipy.stats.hmean(df[speedup].values)
    pgmq = scipy.stats.mstats.gmean(df[speedup].values)
    print('Harmonic mean (top 25%): ', phmq)
    print('Geometric mean (top 25%): ', pgmq)
    print('Arithmetic mean (top 25%): ', df[speedup].mean())
    plt.savefig('fig6.png', bbox_inches='tight')
