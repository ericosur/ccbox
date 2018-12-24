#!/usr/bin/env python

'''
sample to use numpy to do matrix calculation
'''

import numpy as np

def test0():
    a = np.array([[1,2],[3,4]])
    print(a)
    t = a.transpose()
    print(t)

def test1():
    a = np.matrix('1 1; 2 -1; -2 0; 1 1')
    v = np.matrix('1; 2; 3; 4')
    ans = a.T * a
    print(ans)
    ans = a * a.T
    print(ans)
    try:
        print(ans.I)
    except np.linalg.linalg.LinAlgError as e:
        print('exception caught: {}'.format(e))

def main():
    test0()
    test1()

if __name__ == '__main__':
    main()
