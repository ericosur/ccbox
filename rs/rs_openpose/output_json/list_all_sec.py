#!/usr/bin/env python3

'''
list COCO keypoints from json file
'''

import sys
from glob import glob
from test_proc import TestProc

if __name__ == '__main__':
    if len(sys.argv) == 1:
        files = glob('*Color_keypoints.json')
        for ff in files:
            prefix = TestProc.get_prefix(ff)
            print(prefix)
            tp = TestProc(prefix)
            tp.process_all_sections()
            print
    elif len(sys.argv) == 2:
        tp = TestProc(sys.argv[1])
        tp.process_all_sections()
