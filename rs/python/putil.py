#!/usr/bin/env python3

''' try to insert necessary path into sys.path and say hello '''

__version__ = '0.0.1.2018.8.14'

import os
import sys

import numpy as np
import cv2

def is_path_exist(p):
    return os.path.exists(p)

def insert_syspath(p):
    try:
        idx = sys.path.index(p)
        print('{} is already in sys.path'.format(p))
        return
    except ValueError:
        # not exist at sys.path will insert it
        pass

    if is_path_exist(p):
        print('insert {} into sys path'.format(p))
        sys.path.insert(0, p)


def add_local_lib():
    LOCAL_LIB_DIR = '/usr/local/lib'
    insert_syspath(LOCAL_LIB_DIR)

def main():
    add_local_lib()
    import pyrealsense2 as rs
    print('just import required modules')


if __name__ == '__main__':
    main()
