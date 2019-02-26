#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import math
import cv2
import numpy as np

def draw_bounding_box(frame, marker):
    # draw a bounding box around the image and display it
    #box = np.int0(cv2.cv.BoxPoints(marker))
    box = cv2.boxPoints(marker)
    box = np.int0(box)
    cv2.drawContours(frame, [box], -1, (0, 255, 0), 2)

def px_dist(p1, p2):
    d = math.sqrt((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2)
    return d

def px_3d_dist(p1, p2):
    d = math.sqrt((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2 + (p1[2]-p2[2])**2)
    return d

# ret: corners np.int64
def get_poi(marker):
    # print(type(marker))
    # print(marker.ndim)  # number of dimensions
    # print(marker.shape) # m*n
    # print(marker.dtype) # data type
    idxs = [0, 6, 62, 56]
    pts = marker.astype(np.int64)
    corners = []
    for ii in idxs:
        (u, v) = pts[ii][0]
        corners.append((u, v))
    return corners

def get_ppts(marker):
    #id = [0, 6, 62, 56]
    ppts = np.array([marker[0], marker[6], marker[62], marker[56]], dtype="float32")
    #print("ppts:{}".format(ppts))
    return ppts

def draw_line_and_dist(img, p1, p2, color=(0,127,255)):
    #print("dist: {:.2f}".format(px_dist(p1, p2)))
    cv2.line(img, p1, p2, color, 2) # orange

def draw_poi(img, corners):
    rr = 5
    prev = None
    pp0 = None
    for i, cc in enumerate(corners):
        cv2.circle(img, cc, rr, (255,255,255), -1)
        if pp0 is None:
            pp0 = cc
        if prev is not None:
            draw_line_and_dist(img, prev, cc, (255,255/(i+1),255))
        prev = cc

    draw_line_and_dist(img, cc, pp0, (127,127,255))

def get_perspective_mat(src_points, dst_w, dst_h):
    #src_points = np.array([[270., 69.], [690.,164.], [273.,458.], [694.,518.]], dtype="float32")
    dst_points = np.array([[0.,0.], [dst_w-1.,0.],
                           [dst_w-1.,dst_h-1],
                           [0.,dst_h-1.]],
                           dtype="float32")
    M = cv2.getPerspectiveTransform(src_points, dst_points)
    return M

def getmm(p):
    q = []
    for pp in p:
        q.append(pp*1000)
    return q

def put_mytext(img, text):
    cv2.putText(img, text, (img.shape[1] - 500, img.shape[0] - 20),
                cv2.FONT_HERSHEY_SIMPLEX, 1.1, (0, 255, 255), 3)

