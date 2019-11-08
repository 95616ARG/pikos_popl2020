#!/usr/bin/env python3
import subprocess

BUCKETNAME1 = 's3://ikos'

def sync():
    subprocess.call(['mkdir', 'tmp'])
    subprocess.call(['cp', '-r', 'xml', 'tmp'])
    subprocess.call(['aws', 's3', 'cp', '--recursive', 'tmp', BUCKETNAME1])
    subprocess.call(['rm', '-r', 'tmp'])

if __name__ == '__main__':
    # Sync s3://ikos with xml
    sync()
