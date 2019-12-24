#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#import the necessary packages
import os
import sys
from glob import glob
import json
import math
import numpy as np
import cv2
#import pyrealsense2 as rs
from myutil import read_jsonfile, write_json, isfile

# 找到目标函数
def find_marker(image):
    # convert the image to grayscale, blur it, and detect edges
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    edged = cv2.Canny(gray, 35, 125)

    # find the contours in the edged image and keep the largest one;
    # we'll assume that this is our piece of paper in the image
    #(_, cnts, _) = cv2.findContours(edged.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    (cnts, hirachy) = cv2.findContours(edged.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    # 求最大面积
    c = max(cnts, key = cv2.contourArea)

    # compute the bounding box of the of the paper region and return it
    # cv2.minAreaRect() c代表点集，返回rect[0]是最小外接矩形中心点坐标，
    # rect[1][0]是width，rect[1][1]是height，rect[2]是角度
    return cv2.minAreaRect(c)

# 距离计算函数
def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth


def draw_bounding_box(frame, marker):
    # draw a bounding box around the image and display it
    #box = np.int0(cv2.cv.BoxPoints(marker))
    box = cv2.boxPoints(marker)
    box = np.int0(box)
    cv2.drawContours(frame, [box], -1, (0, 255, 0), 2)


def main():
    ''' main '''
    JSONFILE = 'ircam.json'
    jd = read_jsonfile(JSONFILE)
    if jd:
        cnt = jd['last_count']
        print('last_count:', cnt)
        cnt += 1
    else:
        cnt = 0

    # initialize the known distance from the camera to the object, which
    # preset distance from samples
    KNOWN_DISTANCE = 390

    # initialize the known object width, which in this case, the piece of
    # paper, unit mm
    KNOWN_WIDTH = 252
    KNOWN_HEIGHT = 201

    # load the furst image that contains an object that is KNOWN TO BE 2 feet
    # from our camera, then find the paper marker in the image, and initialize
    # the focal length
    home = os.environ["HOME"]
    fn = home + '/Pictures/data/pic0001.png'
    if not isfile(fn):
        print('[ERROR] {} not found...'.format(fn))
        sys.exit(1)
    image = cv2.imread(fn)
    marker = find_marker(image)
    focalLength = (marker[1][0] * KNOWN_DISTANCE) / KNOWN_WIDTH
    draw_bounding_box(image, marker)
    print('output reference image to focal.png')
    cv2.imwrite('focal.png', image)

    #通过摄像头标定获取的像素焦距
    #focalLength = 811.82
    print('focalLength = ',focalLength)
    #return

    # /dev/video1 (RGB)
    # /dev/video2 (IR)
    rgb_cam = cv2.VideoCapture(0)
    ir_cam = cv2.VideoCapture(2)

    while ir_cam.isOpened():
        # get a frame
        (rgb_grabbed, rgb_frame) = rgb_cam.read()
        (ir_grabbed, ir_frame) = ir_cam.read()

        frame = None
        # show a frame
        if rgb_grabbed:
            frame = rgb_frame.copy()
            cv2.imshow("rgb", frame)
        if ir_grabbed:
            cv2.imshow("ir", ir_frame)

        key = cv2.waitKey(1)
        if key & 0xFF == ord('q') or key == 0x1B:
            break
        elif key & 0xFF == ord('s'):
            rgbf = 'rgb{:04d}.png'.format(cnt)
            irf = 'ir{:04d}.png'.format(cnt)
            cv2.imwrite(rgbf, rgb_frame)
            cv2.imwrite(irf, ir_frame)
            print('output to: {} {}'.format(rgbf, irf))
            jd['last_count'] = cnt
            cnt += 1

    rgb_cam.release()
    ir_cam.release()
    cv2.destroyAllWindows()
    write_json(JSONFILE, jd)


if __name__ == '__main__':
    main()
