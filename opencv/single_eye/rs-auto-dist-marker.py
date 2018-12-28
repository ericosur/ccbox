#!/usr/bin/python3
# -*- coding: utf-8 -*-

#import the necessary packages
from glob import glob
import math
import numpy as np
import cv2
import pyrealsense2 as rs

# find marker
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

# get distance from known width and focal length
def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth


def draw_bounding_box(frame, marker):
    # draw a bounding box around the image and display it
    #box = np.int0(cv2.cv.BoxPoints(marker))
    box = cv2.boxPoints(marker)
    #print('box:{} {}'.format(type(box), box))
    box = np.int0(box)
    #print('box: {}'.format(box))
    cv2.drawContours(frame, [box], -1, (0, 255, 0), 2)



def main():
    # initialize the known distance from the camera to the object, which
    # preset distance from samples
    KNOWN_DISTANCE = 390

    # initialize the known object width, which in this case, the piece of
    # paper, unit mm
    KNOWN_WIDTH = 252
    KNOWN_HEIGHT = 201
    window_name = 'capture'

    '''
    image = cv2.imread(fn)
    marker = find_marker(image)
    #通过摄像头标定获取的像素焦距
    focalLength = (marker[1][0] * KNOWN_DISTANCE) / KNOWN_WIDTH
    print('focalLength = ',focalLength)
    draw_bounding_box(image, marker)
    print('output reference image to focal.png')
    cv2.imwrite('focal.png', image)
    '''

    # Configure depth and color streams
    pipeline = rs.pipeline()
    config = rs.config()
    # 640x480, 640x360
    config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
    config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)

    # Start streaming
    pipeline.start(config)

    cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
    cv2.moveWindow(window_name, 0, 0)

    cnt = 0

    try:
        while True:
            # Wait for a coherent pair of frames: depth and color
            frames = pipeline.wait_for_frames()
            depth_frame = frames.get_depth_frame()
            color_frame = frames.get_color_frame()
            if not depth_frame or not color_frame:
                continue

            # Convert images to numpy arrays
            color_image = np.asanyarray(color_frame.get_data())
            #depth_image = np.asanyarray(depth_frame.get_data())

            # Apply colormap on depth image (image must be converted to 8-bit per pixel first)
            #depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.03), cv2.COLORMAP_JET)


            original = color_image.copy()
            marker = find_marker(color_image)
            if marker == 0:
                print(marker)
                continue

            #dist = distance_to_camera(KNOWN_WIDTH, focalLength, marker[1][0])

            draw_bounding_box(color_image, marker)

            depth_intrinsics = rs.video_stream_profile(depth_frame.profile).get_intrinsics()
            # u, v is 2D pixel coordinate
            # return p is 3D (x, y, z)
            def get_point(u, v):
                d = depth_frame.get_distance(u, v)
                p = rs.rs2_deproject_pixel_to_point(depth_intrinsics, [u, v], d)
                return p

            def dist_3d(p1, p2):
                d = math.sqrt(math.pow(p1[0]-p2[0], 2) +
                              math.pow(p1[1]-p2[1], 2) +
                              math.pow(p1[2]-p2[2], 2));
                return d

            def get_marker_xyz(marker):
                u = int(marker[0][0])
                v = int(marker[0][1])
                p0 = get_point(u, v)
                u = int(marker[1][0])
                v = int(marker[1][1])
                p1 = get_point(u, v)
                d = dist_3d(p0, p1)
                return d

            d = get_marker_xyz(marker) * 1000
            dist_from_rs = depth_frame.get_distance(int(marker[0][0]), int(marker[0][1]))
            dist_from_rs *= 1000

            #print('marker:{} dist_rs:{}'.format(marker, dist_from_rs))
            # cv2.putText(color_image, "%4.0fmm" % dist,
            #             (color_image.shape[1] - 500, color_image.shape[0] - 60),
            #             cv2.FONT_HERSHEY_SIMPLEX,
            #             1.2, (0, 255, 0), 3)

            cv2.putText(color_image, "p0d: %4dmm" % dist_from_rs,
                        (color_image.shape[1] - 500, color_image.shape[0] - 60),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        1.2, (0, 255, 255), 3)
            cv2.putText(color_image, "w: %4dmm" % d,
                        (color_image.shape[1] - 500, color_image.shape[0] - 20),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        1.2, (0, 255, 127), 3)

            # show a frame
            cv2.imshow(window_name, color_image)
            key = cv2.waitKey(1)
            if key & 0xFF == ord('q') or key == 0x1B:
                break
            elif key & 0xFF == ord('s'):
                fn = 'result-{:02d}.png'.format(cnt)
                print('save result to {}'.format(fn))
                cv2.imwrite(fn, color_image)
                cv2.imwrite('origin-{:02d}.png'.format(cnt), original)
                cnt += 1
    finally:
        cv2.destroyAllWindows()
        # Stop streaming
        pipeline.stop()


if __name__ == '__main__':
    main()
