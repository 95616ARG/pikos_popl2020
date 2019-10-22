#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import pandas as pd
import scipy.stats
import scipy.stats.mstats
import sys

if __name__ == '__main__':
    mpl.rcParams['text.usetex'] = True
    mpl.rcParams['text.latex.preamble'] = [r'\usepackage{amsmath}']
    mpl.rcParams['font.size'] = 15.0
    mpl.rcParams['axes.labelsize'] = 17.0
    mpl.rcParams['ytick.labelsize'] = 17.0
    mpl.rcParams['xtick.labelsize'] = 17.0
    df = pd.read_csv(sys.argv[1])

    pdf4 = df[df['status-4'] == 'OK']
    pb4 = pdf4['walltime (s)'].sum()
    ps4 = pdf4['walltime (s)-4'].sum()
    pd4 = pdf4['speedup-4']

    pdf6 = df[df['status-6'] == 'OK']
    pb6 = pdf6['walltime (s)'].sum()
    ps6 = pdf6['walltime (s)-6'].sum()
    pd6 = pdf6['speedup-6']

    pdf8 = df[df['status-8'] == 'OK']
    pb8 = pdf8['walltime (s)'].sum()
    ps8 = pdf8['walltime (s)-8'].sum()
    pd8 = pdf8['speedup-8']

    pdata = [pd4.values, pd6.values, pd8.values] 
    labels = ['NT=4', 'NT=6', 'NT=8']
    phmean = list(map(lambda x: scipy.stats.hmean(x), pdata))
    pgmean = list(map(lambda x: scipy.stats.mstats.gmean(x), pdata))
    pamean = list(map(lambda x: sum(x) / len(x), pdata))
    x = [4,6,8]

    
    fig, (ax1, ax2) = plt.subplots(nrows=1, ncols=2, figsize=(9, 4), sharey=True)
    ax1.set_xlabel('Number of threads, $k$')
    ax1.set_ylabel(r'Observed speedups of $\textsc{Pikos\textlangle{}k\textrangle{}}$')
    ax1.grid(b=True, ls='--', alpha=0.5)
    ax1.boxplot(pdata, positions=x, sym='k.')
    ax1.scatter(x, pamean, marker='.',  c='r', zorder=1000, label="Arithmetic mean")
    ax1.scatter(x, pgmean, marker='*',  c='g', zorder=1000, label="Geometric mean")
    ax1.scatter(x, phmean, marker='x',  c='b', zorder=1000, label="Harmonic mean")
    print("Arith, Geo, Harmonic")
    print(pamean, pgmean, phmean)
    ax1.legend(loc='upper left')

    ax2.set_xlabel('Number of threads, $k$')
    ax2.grid(b=True, ls='--', alpha=0.5)
    ax2.violinplot(pdata, positions=x, showextrema=True, showmeans=False, showmedians=False, widths=0.8, points=1000)
    ax2.scatter(x, pamean, marker='.',  c='r', zorder=1000, label="Arithmetic mean")
    ax2.scatter(x, pgmean, marker='*',  c='g', zorder=1000, label="Geometric mean")
    ax2.scatter(x, phmean, marker='x',  c='b', zorder=1000, label="Harmonic mean")
    ax2.legend(loc='upper left')
    ax2.set_xticks(x)

    plt.subplots_adjust(bottom=0.15, wspace=0.05)
    plt.savefig('fig8.png', bbox_inches='tight')
