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
    mpl.rcParams['ytick.labelsize'] = 15.0
    mpl.rcParams['xtick.labelsize'] = 15.0
    mpl.rcParams['font.weight'] = 'bold'
    df = pd.read_csv(sys.argv[1])
    base = 'walltime (s)'
    ours = 'walltime (s)-4'
    speedup = 'speedup-4'
    df = df[df[base] >= 5]
    df = df.sort_values(by=base)
    r, c = df.shape
    d = r // 4

    for i in range(4):
        dff = None
        if i == 3:
            dff = df[d*3:]
        else:
            dff = df[d*i:d*(i+1)]
        ax = plt.subplot(111)
    
        n, bins, patches = ax.hist(dff[speedup].values, bins=12, range=[0.75,3.75], edgecolor='black', alpha=0.8)
        plt.yticks(list(plt.yticks()[0]) + [1, max(n)])
        plt.yticks([0, 1, 10, 20, 30, 40, 50, 60, max(n)])
    
        plt.grid(b=True, zorder=0, ls='--', alpha=0.3)
        plt.xlabel(r'Speedup of $\textsc{Pikos\textlangle4\textrangle}$ (higher the better)')
        plt.ylabel(r'Frequency')
    
    
        if i > 1:
            plt.text(0.05, 0.95, "Max. speedup = %.2fx\nMin. speedup = %.2fx" % (round(dff[speedup].max(), 2), round(dff[speedup].min(), 2)),
                    transform=ax.transAxes, verticalalignment='top')
        else:
            plt.text(0.55, 0.95, "Max. speedup = %.2fx\nMin. speedup = %.2fx" % (round(dff[speedup].max(), 2), round(dff[speedup].min(), 2)),
                    transform=ax.transAxes, verticalalignment='top')
    
        plt.savefig('fig7-%d.png' % i, bbox_inches='tight')
        plt.clf()
