#!/usr/bin/env python3

# Partitions 'all.set' and 'tab3.set' into batches in 'batches/'.

import sys
import math

dummy = './dummy.bc\n'
nbatches = 200 # Number of batches.
allset = 'all.set' # Required to generate all.csv
tab3set = 'tab3.set' # Required to generate tab 3

def line2entry(line):
    name, cs = line.strip().split(',')
    return '''    <tasks>
      <option name="-cs">%s</option>
      <include>%s</include>
    </tasks>''' % (cs, name)

if __name__ == '__main__':
    with open(allset) as f:
        l = f.readlines()
        n = math.ceil(len(l) / nbatches) # Number of benchmarks in a batch
        with open('batch-b24-template.xml', 'r') as t_b24:
            with open('batch-68-template.xml', 'r') as t_68:
                t_b24 = t_b24.read()
                t_68 = t_68.read()
                for i in range(min(nbatches, len(l))):
                    partition = l[i*n : (i+1)*n]
                    tasks = '\n'.join(map(line2entry, partition))
                    with open('batches/batch-b24-%d.xml' % i, 'w') as ff:
                        ff.write(t_b24.replace('{tasks}', tasks))
                    with open('batches/batch-68-%d.xml' % i, 'w') as ff:
                        ff.write(t_68.replace('{tasks}', tasks))
    with open(tab3set) as f:
        l = f.readlines()
        n = math.ceil(len(l) / nbatches) # Number of benchmarks in a batch
        with open('batch-1216-template.xml', 'r') as t_1216:
            t_1216 = t_1216.read()
            for i in range(min(nbatches, len(l))):
                partition = l[i*n : (i+1)*n]
                tasks = '\n'.join(map(line2entry, partition))
                with open('batches/batch-1216-%d.xml' % i, 'w') as ff:
                    ff.write(t_1216.replace('{tasks}', tasks))
