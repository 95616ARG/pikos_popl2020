#!/usr/bin/env python3

# Merges the results of the batches.

import subprocess
import boto3
import time
import os
import glob
import pandas as pd
import datetime
import sys

def validxml(x):
    return (x.endswith('base.xml.bz2') or
            x.endswith('cc2.xml.bz2') or
            x.endswith('cc4.xml.bz2') or
            x.endswith('cc6.xml.bz2') or
            x.endswith('cc8.xml.bz2') or
            x.endswith('cc12.xml.bz2') or
            x.endswith('cc16.xml.bz2'))

if __name__ == '__main__':
    # Remove previously generated csv
    subprocess.call(['rm'] + glob.glob('results-ikos/**/*.csv'))

    # Generate tables
    for r, _, f in os.walk('results-ikos'):
        if r == 'results-ikos':
            continue
        xmls = list(map(lambda x: r+'/'+x, filter(validxml, f)))
        xmls.sort()
        print(xmls)
        subprocess.call(['table-generator', '-f', 'csv'] + xmls)

    # Merge tables
    df = None
    for csv in glob.glob('results-ikos/batch-b24-**/*.table.csv'):
        print(csv)
        new_df = pd.read_csv(csv, skiprows=2, delimiter='\t')
        new_df.drop([0], inplace=True)
        if df is None:
            df = new_df
        else:
            df = df.append(new_df, ignore_index=True)
    df.to_csv('b24.csv', index=False, encoding='utf8')

    df = None
    for csv in glob.glob('results-ikos/batch-68-**/*.table.csv'):
        print(csv)
        new_df = pd.read_csv(csv, skiprows=2, delimiter='\t')
        new_df.drop([0], inplace=True)
        if df is None:
            df = new_df
        else:
            df = df.append(new_df, ignore_index=True)
    df.to_csv('68.csv', index=False, encoding='utf8')

    df = None
    for csv in glob.glob('results-ikos/batch-1216-**/*.table.csv'):
        print(csv)
        new_df = pd.read_csv(csv, skiprows=2, delimiter='\t')
        new_df.drop([0], inplace=True)
        if df is None:
            df = new_df
        else:
            df = df.append(new_df, ignore_index=True)
    df.to_csv('1216.csv', index=False, encoding='utf8')
