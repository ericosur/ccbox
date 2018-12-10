#!/usr/bin/env python
# -*- coding: utf-8 -*-

''' opencv python example '''

from __future__ import print_function
import os
import sys
import cv2
import numpy as np
import myutil

SETTING_FN = 'homo.json'
WIN_NAME = 'foobar'

def init_window():
    cv2.namedWindow(WIN_NAME)
    cv2.moveWindow(WIN_NAME, 0, 0)

def load_setting(fn):
    if not myutil.isfile(fn):
        return None, None

    data = myutil.read_jsonfile(SETTING_FN)
    #print(data['fn'])
    fn = data.get('fn')
    print('image fn: {}'.format(fn))
    frame = np.zeros((640, 480, 3), np.uint8)
    if myutil.isfile(fn):
        frame = cv2.imread(fn)
    else:
        print('image file not found: {}'.format(fn))

    rect = data['rect']
    rect = tuple(map(np.int16, rect))
    print('preset loaded')

    return frame, rect

def main():
    '''main function'''

    init_window()

    img = None
    # cli argument go first
    if len(sys.argv) > 1:   # has argument
        for ff in sys.argv[1:]:
            print('imread {}'.format(ff))
            img = cv2.imread(ff)
            cv2.imshow(WIN_NAME, img)
    else:
        img, rect = load_setting(SETTING_FN)
        print(type(img))
        if img is not None:
            if rect is not None:
                print(rect)
                cv2.rectangle(img, (rect[0], rect[1]), (rect[2], rect[3]), (0, 255, 0), 2)
            cv2.imshow(WIN_NAME, img)

    cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
