#!/usr/bin/python3
# -*- coding: utf-8 -*-

#import the necessary packages
from glob import glob
import math
import numpy as np
import cv2

# checker board size 7 by 9
SIZE = (7, 9)

# size that restored from distortion
DST_W = 280
DST_H = 360

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


def get_perspective_mat(src_points):
    #src_points = np.array([[270., 69.], [690.,164.], [273.,458.], [694.,518.]], dtype="float32")
    dst_points = np.array([[0.,0.], [DST_W-1.,0.], [DST_W-1.,DST_H-1], [0.,DST_H-1.]], dtype="float32")
    M = cv2.getPerspectiveTransform(src_points, dst_points)
    return M


def main():
    window_name = 'capture'
    result_name = 'perspective'

    try:
        cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(window_name, 0, 0)
        cv2.namedWindow(result_name, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(result_name, 640, 0)

        cnt = 0

        # open webcam
        camera = cv2.VideoCapture(0)

        while camera.isOpened():
            # get a frame
            (grabbed, frame) = camera.read()
            image = frame.copy()

            #image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
            ret, crnrs = cv2.findChessboardCorners(image, SIZE, cv2.CALIB_CB_FAST_CHECK)
            #cv2.drawChessboardCorners(color_image, SIZE, crnrs, ret)

            if ret:
                fourc = get_poi(crnrs)
                draw_poi(image, fourc)

                cppts = get_ppts(crnrs)
                M = get_perspective_mat(cppts)
                perspective = cv2.warpPerspective(image, M, (DST_W, DST_H), cv2.INTER_LINEAR)
                cv2.imshow(result_name, perspective)

            # show a frame
            cv2.imshow(window_name, image)

            key = cv2.waitKey(1)
            if key & 0xFF == ord('q') or key == 0x1B:
                break
            elif key & 0xFF == ord('s'):
                fn = 'result-{:02d}.png'.format(cnt)
                print('save result to {}'.format(fn))
                cv2.imwrite(fn, color_image)
                cv2.imwrite('origin-{:02d}.png'.format(cnt), original)
                cnt += 1
            elif key == 0x20:
                cv2.waitKey(0)
    finally:
        cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
