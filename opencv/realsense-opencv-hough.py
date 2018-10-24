#!/usr/bin/env python3

## License: Apache 2.0. See LICENSE file in root directory.
## Copyright(c) 2015-2017 Intel Corporation. All Rights Reserved.

###############################################
##      Open CV and Numpy integration        ##
###############################################

import math
import pyrealsense2 as rs
import numpy as np
import cv2

def dist_2d(p1, p2):
    return ((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

def show_fps(img, elapsed_time):
    ''' put text of elapse time and FPS from input image '''
    fontface = cv2.FONT_HERSHEY_SIMPLEX
    scale = 0.75
    #baseline = 0
    #thickness = 2
    fps = 1.0 / elapsed_time
    msg = "elapsed: {:.3f} fps({:.1f})".format(elapsed_time, fps)
    cv2.putText(img, msg, (10, 35), fontface, scale, (127, 0, 255), 1, cv2.LINE_AA)
    return img


def slope(p1, p2):
    delta_x = float(p2[0] - p1[0])
    delta_y = float(p2[1] - p1[1])
    if delta_x == 0:
        return -1.0
    return math.fabs(delta_y / delta_x)


def try_image(img):
    # value could be + or - for brightness or darkness
    brt = 50
    # change any value in the 2D list < max limit
    img[img < 255 - brt] += brt
    return img

def check_line(p1, p2):
    s = slope(p1, p2)
    if s > 0.0 and s < 0.5777:
        return True
    return False

def hough_lines(src):
    ''' reference from opencv python examples houghlines.py '''
    p0 = (0, 0)

    canny = cv2.Canny(src, 50, 200)
    #print('canny.shape', canny.shape)
    #cv2.imshow('test', dst)

    cdst = cv2.cvtColor(canny, cv2.COLOR_GRAY2BGR)
    #print('cdst.shape', cdst.shape)
    #return cdst, (p0, p0)

    ret = None
    num_lines = 0
    ok_idx = []
    lines = cv2.HoughLinesP(canny, 1, math.pi/180.0, 40, np.array([]), 50, 10)
    try:
        a, b, c = lines.shape
        num_lines = a
        for i in range(a):
            p1 = (lines[i][0][0], lines[i][0][1])
            p2 = (lines[i][0][2], lines[i][0][3])
            cv2.line(cdst, p1, p2, (255, 255, 0), 1, cv2.LINE_AA)
            if check_line(p1, p2):
                #print("{}: {}".format(i, s))
                ok_idx.append(i)
    except AttributeError:
        #print('.', end='')
        return cdst, (p0, p0)


    #print('len(ok_idx)', len(ok_idx))
    try:
        ok_dists = []
        for ii in ok_idx:
            p1 = (lines[ii][0][0], lines[ii][0][1])
            p2 = (lines[ii][0][2], lines[ii][0][3])
            if check_line(p1, p2):
                # this is a problem here
                d = dist_2d(p1, p2)
                ok_dists.append(d)

        imax = max(ok_dists)
        longest_idx = ok_dists.index(imax)
        p1 = (lines[longest_idx][0][0], lines[longest_idx][0][1])
        p2 = (lines[longest_idx][0][2], lines[longest_idx][0][3])
        print("longest_idx:{} imax:{} slope:{}".format(longest_idx, imax, slope(p1, p2)))
        cv2.line(cdst, p1, p2, (255, 0, 255), 2, cv2.LINE_AA)
    except ValueError:
        pass

    #cv2.imshow("test", cdst)
    return cdst, (p1, p2)


def process_image(src):
    e0 = cv2.getTickCount()
    ifrm = src.copy()
    blur = cv2.GaussianBlur(ifrm, (5, 5), 1);
    cvimg, (p1, p2) = hough_lines(ifrm)
    e1 = cv2.getTickCount()
    elapsed = (e1 - e0) / cv2.getTickFrequency()
    show_fps(cvimg, elapsed)
    #print("line: ", p1, p2, slope(p1, p2))
    #blue = self.split_blue(ifrm)
    #cv2.imshow('test', cvimg)
    return blur, cvimg


def main():
    # Configure depth and color streams
    pipeline = rs.pipeline()
    config = rs.config()
    config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
    config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)

    # Start streaming
    pipeline.start(config)

    cv2.namedWindow('RealSense', cv2.WINDOW_AUTOSIZE)
    cv2.moveWindow('RealSense', 0, 0)

    try:
        while True:

            # Wait for a coherent pair of frames: depth and color
            frames = pipeline.wait_for_frames()
            depth_frame = frames.get_depth_frame()
            color_frame = frames.get_color_frame()
            if not depth_frame or not color_frame:
                continue

            # Convert images to numpy arrays
            depth_image = np.asanyarray(depth_frame.get_data())
            color_image = np.asanyarray(color_frame.get_data())

            # Apply colormap on depth image (image must be converted to 8-bit per pixel first)
            depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.03), cv2.COLORMAP_JET)

            #cimg = try_image(color_image)
            #cimg = cv2.cvtColor(color_image, cv2.COLOR_RGB2GRAY)

            cimg, himg = process_image(color_image)
            # Stack both images horizontally
            #images = np.hstack((color_image, depth_colormap))
            #images = np.hstack((color_image, himg))
            images = np.hstack((cimg, himg))

            # Show images
            cv2.imshow('RealSense', images)
            if cv2.waitKey(1) == 27:
                break
            # if cv2.waitKey(1) == 27:
            #     break

    finally:
        cv2.destroyAllWindows()
        # Stop streaming
        pipeline.stop()

if __name__ == '__main__':
    main()
