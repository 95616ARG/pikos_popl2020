#!/usr/bin/env python3
import subprocess
import boto3
import time
import os
import glob
import pandas as pd
import datetime
import sys

if __name__ == '__main__':
    # Generate tables
    f = glob.glob('results-ikos/**/*.csv')
    subprocess.call(['rm'] + f)
    f = glob.glob('results-ikos/**/*.html')
    subprocess.call(['rm'] + f)
    for r, _, f in os.walk('results-ikos'):
        if r == 'results-ikos':
            continue
        f =  filter(lambda x: x.endswith('xml.bz2'), f)
        f = list(map(lambda x: r+'/'+x, f))
        f.sort()
        print(f)
        subprocess.call(['table-generator'] + f)

    # Merge tables
    df = None
    for csv in glob.glob('results-ikos/**/*table.csv'):
        subprocess.call(['sed', '-i', 's/bitcode-files\///', csv])
        subprocess.call(['sed', '-i', 's/sv-benchmarks\///', csv])
        subprocess.call(['sed', '-i', 's/^results-ikos\//filename/', csv])
        print(csv)
        new_df = pd.read_csv(csv, skiprows=2, delimiter='\t')
        new_df.drop([0], inplace=True)
        if df is None:
            df = new_df
        else:
            df = df.append(new_df, ignore_index=True)

    df.to_csv('unprocessed.csv', index=False, encoding='utf8')
