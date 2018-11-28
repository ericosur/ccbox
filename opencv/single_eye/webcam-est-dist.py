#!/usr/bin/python
# -*- coding: utf-8 -*-

#import the necessary packages
from glob import glob
import math
import numpy as np
import cv2
#import pyrealsense2 as rs

# 找到目标函数
def find_marker(image):
    # convert the image to grayscale, blur it, and detect edges
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    edged = cv2.Canny(gray, 35, 125)

    # find the contours in the edged image and keep the largest one;
    # we'll assume that this is our piece of paper in the image
    (_, cnts, _) = cv2.findContours(edged.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
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
    # initialize the known distance from the camera to the object, which
    # preset distance from samples
    KNOWN_DISTANCE = 390

    # initialize the known object width, which in this case, the piece of
    # paper, unit mm
    KNOWN_WIDTH = 252
    KNOWN_HEIGHT = 201

    # initialize the list of images that we'll be using
    IMAGE_PATHS = ["pic0001.png", "pic0002.png", "pic0003.png"]

    # load the furst image that contains an object that is KNOWN TO BE 2 feet
    # from our camera, then find the paper marker in the image, and initialize
    # the focal length
    #读入第一张图，通过已知距离计算相机焦距
    fn = IMAGE_PATHS[0]
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

    #打开摄像头
    camera = cv2.VideoCapture(0)

    while camera.isOpened():
        # get a frame
        (grabbed, frame) = camera.read()
        original = frame.copy()
        marker = find_marker(frame)
        if marker == 0:
            print(marker)
            continue

        dist = distance_to_camera(KNOWN_WIDTH, focalLength, marker[1][0])

        draw_bounding_box(frame, marker)

        # inches 转换为 cm
        cv2.putText(frame, "%.2fmm" % dist,
                    (40, frame.shape[0] - 20),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    1.2, (0, 255, 0), 3)

        # show a frame
        cv2.imshow("capture", frame)
        key = cv2.waitKey(1)
        if key & 0xFF == ord('q') or key == 0x1B:
            break
        elif key & 0xFF == ord('s'):
            print('save result to result.png')
            cv2.imwrite('result.png', frame)
            cv2.imwrite('origin.png', original)

    camera.release()
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
