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
    mpl.rcParams['ytick.labelsize'] = 15.0
    mpl.rcParams['xtick.labelsize'] = 15.0
    mpl.rcParams['font.weight'] = 'bold'
    df = pd.read_csv(sys.argv[1])
    base = 'walltime (s)'
    ours = 'walltime (s)-4'
    speedup = 'speedup-4'
    df = df[df[base] >= 5]

    total, _ = df.shape
    above2, _ = df[df[speedup] >= 2].shape
    above3, _ = df[df[speedup] >= 3].shape
    print(above2, above2 / total, '%')
    print(above3, above3 / total , '%')

    q25, q50, q75 = df[base].quantile([0.25, 0.5, 0.75])
    print("Quartile 1: ", q25)
    print("Quartile 2: ", q50)
    print("Quartile 3: ", q75)

    df0 = df[df[base] < q25]
    print(df0.shape)
    ax = plt.subplot(111)
    print(df0.sort_values(by=base).iloc[[0, -1]][[base]])
    n, bins, patches = ax.hist(df0[speedup].values, bins=12, range=[0.75,3.75], edgecolor='black', alpha=0.8)
    plt.yticks(list(plt.yticks()[0]) + [1, max(n)])
    plt.yticks([0, 1, 10, 20, 30, 40, 50, 60, max(n)])
    plt.grid(b=True, zorder=0, ls='--', alpha=0.3)
    plt.xlabel(r'Speedup of $\textsc{Pikos\textlangle4\textrangle}$ (higher the better)')
    plt.ylabel(r'Frequency')
    plt.text(0.55, 0.95, "Max. speedup = %.2fx\nMin. speedup = %.2fx" % (round(df0[speedup].max(), 2), round(df0[speedup].min(), 2)),
    transform=ax.transAxes, verticalalignment='top')
    plt.savefig('fig7-%d.png' % 0, bbox_inches='tight')
    plt.clf()

    df1 = df[(q25 <= df[base]) & (df[base] < q50)]
    print(df1.shape)
    ax = plt.subplot(111)
    print(df1.sort_values(by=base).iloc[[0, -1]][[base]])
    n, bins, patches = ax.hist(df1[speedup].values, bins=12, range=[0.75,3.75], edgecolor='black', alpha=0.8)
    plt.yticks(list(plt.yticks()[0]) + [1, max(n)])
    plt.yticks([0, 1, 10, 20, 30, 40, 50, 60, max(n)])
    plt.grid(b=True, zorder=0, ls='--', alpha=0.3)
    plt.xlabel(r'Speedup of $\textsc{Pikos\textlangle4\textrangle}$ (higher the better)')
    plt.ylabel(r'Frequency')
    plt.text(0.55, 0.95, "Max. speedup = %.2fx\nMin. speedup = %.2fx" % (round(df1[speedup].max(), 2), round(df1[speedup].min(), 2)),
    transform=ax.transAxes, verticalalignment='top')
    plt.savefig('fig7-%d.png' % 1, bbox_inches='tight')
    plt.clf()

    df2 = df[(q50 <= df[base]) & (df[base] < q75)]
    print(df2.shape)
    ax = plt.subplot(111)
    print(df2.sort_values(by=base).iloc[[0, -1]][[base]])
    n, bins, patches = ax.hist(df2[speedup].values, bins=12, range=[0.75,3.75], edgecolor='black', alpha=0.8)
    plt.yticks(list(plt.yticks()[0]) + [1, max(n)])
    plt.yticks([0, 1, 10, 20, 30, 40, 50, 60, max(n)])
    plt.grid(b=True, zorder=0, ls='--', alpha=0.3)
    plt.xlabel(r'Speedup of $\textsc{Pikos\textlangle4\textrangle}$ (higher the better)')
    plt.ylabel(r'Frequency')
    plt.text(0.05, 0.95, "Max. speedup = %.2fx\nMin. speedup = %.2fx" % (round(df2[speedup].max(), 2), round(df2[speedup].min(), 2)),
    transform=ax.transAxes, verticalalignment='top')
    plt.savefig('fig7-%d.png' % 2, bbox_inches='tight')
    plt.clf()

    df3 = df[q75 <= df[base]]
    print(df3.shape)
    ax = plt.subplot(111)
    print(df3.sort_values(by=base).iloc[[0, -1]][[base]])
    n, bins, patches = ax.hist(df3[speedup].values, bins=12, range=[0.75,3.75], edgecolor='black', alpha=0.8)
    plt.yticks(list(plt.yticks()[0]) + [1, max(n)])
    plt.yticks([0, 1, 10, 20, 30, 40, 50, 60, max(n)])
    plt.grid(b=True, zorder=0, ls='--', alpha=0.3)
    plt.xlabel(r'Speedup of $\textsc{Pikos\textlangle4\textrangle}$ (higher the better)')
    plt.ylabel(r'Frequency')
    plt.text(0.05, 0.95, "Max. speedup = %.2fx\nMin. speedup = %.2fx" % (round(df3[speedup].max(), 2), round(df3[speedup].min(), 2)),
    transform=ax.transAxes, verticalalignment='top')
    plt.savefig('fig7-%d.png' % 3, bbox_inches='tight')
    plt.clf()

    print('Figures saved to "fig7-[0~3].png".')
