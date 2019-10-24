#!/usr/bin/env python3
import subprocess

def sync():
    subprocess.call(['mkdir', 'tmp'])
    subprocess.call(['cp', '-r', 'xml', 'tmp'])
    subprocess.call(['cp', '-r', 'cs', 'tmp'])
    subprocess.call(['aws', 's3', 'cp', '--recursive', 'tmp', 's3://ikos'])
    subprocess.call(['rm', '-r', 'tmp'])

if __name__ == '__main__':
    # Sync s3://ikos with xml
    sync()
