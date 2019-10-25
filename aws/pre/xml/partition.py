#!/usr/bin/env python3

dummy = './dummy.bc\n'
n = 21 # Number of benchmark in one batch.
inputlist = 'list.txt'

if __name__ == '__main__':
    with open(inputlist) as f:
        l = f.readlines()
        for i in range((len(l) + n - 1) // n):
            partition = l[i*n : (i+1)*n]
            with open('set%d.txt' % i, 'w') as ff:
                ff.write(dummy)
                ff.write(dummy)
                ff.write(''.join(partition))
