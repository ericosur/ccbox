#!/usr/bin/env python3

'''
list COCO keypoints from json file
'''

import sys
from enum import Enum
from glob import glob

import cv2
from myutil import read_jsonfile, isfile
from fc import FindContour
from kp import KP


def process_json(prefix):
    imgfont = cv2.FONT_HERSHEY_SIMPLEX
    orig_path = '../input_rs/'
    imgfile = orig_path + prefix + '_Color.png'
    rawfile = orig_path + prefix + '_Depth.raw'
    jfile = prefix + '_Color_keypoints.json'

    if not isfile(imgfile):
        print('{} not found'.format(imgfile))
        return
    if not isfile(jfile):
        print('{} not found'.format(jfile))
        return
    if not isfile(rawfile):
        print('{} not found'.format(rawfile))
        return

    myctr = FindContour()
    myctr.load_file(imgfile, rawfile)
    p1 = ()
    p2 = ()

    json = read_jsonfile(jfile)
    kps = json['people'][0]['pose_keypoints_2d']

    img = cv2.imread(imgfile)
    for idx, kpn in enumerate(KP):
        x = int(kps[idx * 3])
        y = int(kps[idx * 3 + 1])
        pos = (x, y)
        cv2.circle(img, pos, 3, color=(255, 255, 0))
        imgtext = str(kpn).replace('KP.', '')
        #print('{}: {} - ({},{})'.format(idx, imgtext, x, y))
        cv2.putText(img, imgtext, pos, imgfont, 0.8, (0, 0, 255), 2, cv2.LINE_AA)
        if idx == KP.RIGHT_SHOULDER.value:
            p1 = (x, y)
        if idx == KP.LEFT_SHOULDER.value:
            p2 = (x, y)

    #print('show #1')
    #cv2.imshow('img', img)
    #cv2.waitKey(0)

    #print('imshow from listpts.py')
    img2 = myctr.query_length(p1, p2, img=img)
    cv2.imshow('img', img2)
    cv2.waitKey(0)

def get_prefix(filepath):
    pref = filepath.replace('_Color_keypoints.json', '')
    return pref

if __name__ == '__main__':
    if len(sys.argv) == 1:
        files = glob('*_keypoints.json')
        for ff in files:
            process_json(get_prefix(ff))
    elif len(sys.argv) == 2:
        process_json(sys.argv[1])
