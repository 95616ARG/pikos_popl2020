#!/usr/bin/env python3.6

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
    df = df[df['walltime (s)'] >= 5]

    bsum = df['walltime (s)'].sum()

    cc2sum = df['walltime (s)-2'].sum()
    cc2speedup = df['speedup-2']

    cc4sum = df['walltime (s)-4'].sum()
    cc4speedup = df['speedup-4']

    cc6sum = df['walltime (s)-6'].sum()
    cc6speedup = df['speedup-6']

    cc8sum = df['walltime (s)-8'].sum()
    cc8speedup = df['speedup-8']

    speedups = [cc2speedup.values, cc4speedup.values, cc6speedup.values, cc8speedup.values] 
    labels = ['NT=2', 'NT=4', 'NT=6', 'NT=8']
    hmean = list(map(lambda x: scipy.stats.hmean(x), speedups))
    gmean = list(map(lambda x: scipy.stats.mstats.gmean(x), speedups))
    amean = list(map(lambda x: sum(x) / len(x), speedups))
    x = [2,4,6,8]

    print("Arithematic means of PIKOS<k> (k = 2, 4, 6, 8): ")
    print(amean)
    print("Geomentric means of PIKOS<k> (k = 2, 4, 6, 8): ")
    print(gmean)
    print("Harmonic means of PIKOS<k> (k = 2, 4, 6, 8): ")
    print(hmean)
    
    fig, ax = plt.subplots(nrows=1, ncols=1, figsize=(5, 4), sharey=True)
    ax.set_xlabel('Number of threads $k$')
    ax.set_ylabel(r'Observed speedups of \textsc{Pikos}\textlangle{}$k$\textrangle{}')
    ax.grid(b=True, ls='--', alpha=0.5)
    ax.boxplot(speedups, positions=x, sym='k.')
    ax.scatter(x, amean, marker='.',  c='r', zorder=1000, label="Arithmetic mean")
    ax.scatter(x, gmean, marker='*',  c='g', zorder=1000, label="Geometric mean")
    ax.scatter(x, hmean, marker='x',  c='b', zorder=1000, label="Harmonic mean")
    ax.legend(loc='upper left')
    ax.set_xticks(x)
    ax.set_yticks([2,4,6,8])
    plt.savefig('fig8-a.png', bbox_inches='tight')

    fig, ax = plt.subplots(nrows=1, ncols=1, figsize=(5, 4), sharey=True)
    ax.set_xlabel('Number of threads $k$')
    ax.set_ylabel(r'Observed speedups of \textsc{Pikos}\textlangle{}$k$\textrangle{}')
    ax.grid(b=True, ls='--', alpha=0.5)
    ax.violinplot(speedups, positions=x, showextrema=True, showmeans=False, showmedians=False, widths=0.8, points=1000)
    ax.scatter(x, amean, marker='.',  c='r', zorder=1000, label="Arithmetic mean")
    ax.scatter(x, gmean, marker='*',  c='g', zorder=1000, label="Geometric mean")
    ax.scatter(x, hmean, marker='x',  c='b', zorder=1000, label="Harmonic mean")
    ax.legend(loc='upper left')
    ax.set_xticks(x)
    ax.set_yticks([2,4,6,8])
    plt.savefig('fig8-b.png', bbox_inches='tight')
    print('\nFigures saved to "fig8-a.png" and "fig8-b.png".')
