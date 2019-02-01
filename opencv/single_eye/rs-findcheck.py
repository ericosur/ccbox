#!/usr/bin/python3
# -*- coding: utf-8 -*-

#import the necessary packages
from glob import glob
import math
import numpy as np
import cv2
import pyrealsense2 as rs

SIZE = (7, 9)
#BAG_FILE = '/home/rasmus/Documents/checker.bag'
BAG_FILE = '/home/rasmus/Documents/bucket.bag'

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


def getmm(p):
    q = []
    for pp in p:
        q.append(pp*1000)
    return q


def get_poi_rs_dist(fourc, depth_frame, depth_intrinsics):
    fourxyz = []
    for cc in fourc:
        #z = depth_frame.get_distance(cc[0], cc[1])
        #z *= 1000
        #print("(u, v, z) = ({}, {}, {:.2f})".format(cc[0], cc[1], z))
        u = cc[0]
        v = cc[1]
        p = uv_to_xyz(u, v, depth_frame, depth_intrinsics)
        q = getmm(p)    # q is (x,y,z) in mm
        #print("(x,y,z) = ({:.2f},{:.2f},{:.2f}".format(p[0], p[1], [2]))
        #print("{:.2f},{:.2f},{:.2f}".format(q[0],q[1],q[2]))
        fourxyz.append(q)
    print("[0-1] uv({:.2f}) xyz({:.2f})".format(px_dist(fourc[0], fourc[1]), px_3d_dist(fourxyz[0], fourxyz[1])))
    print("[1-2] uv({:.2f}) xyz({:.2f})".format(px_dist(fourc[1], fourc[2]), px_3d_dist(fourxyz[1], fourxyz[2])))

    return fourxyz


# u, v is 2D pixel coordinate
# return p is 3D (x, y, z)
def uv_to_xyz(u, v, depth_frame, depth_intrinsics):
    d = depth_frame.get_distance(u, v)
    p = rs.rs2_deproject_pixel_to_point(depth_intrinsics, [u, v], d)
    return p


def main():
    window_name = 'capture'
    USE_REAL_CAMERA = False
    try:
        # Configure depth and color streams
        pipeline = rs.pipeline()
        config = rs.config()

        if USE_REAL_CAMERA:
            config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
            config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)
        else:
            config.enable_device_from_file(BAG_FILE)

        # Start streaming
        pipeline.start(config)

        cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(window_name, 0, 0)

        cnt = 0

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

            # Grab new intrinsics (may be changed by decimation)
            depth_intrinsics = rs.video_stream_profile(depth_frame.profile).get_intrinsics()


            original = color_image.copy()
            color_image = cv2.cvtColor(color_image, cv2.COLOR_RGB2BGR)
            ret, crnrs = cv2.findChessboardCorners(color_image, SIZE, cv2.CALIB_CB_FAST_CHECK)
            #cv2.drawChessboardCorners(color_image, SIZE, crnrs, ret)

            if ret:
                fourc = get_poi(crnrs)
                #print(fourc)
                draw_poi(color_image, fourc)
                get_poi_rs_dist(fourc, depth_frame, depth_intrinsics)

            if False:
                dist_from_rs = depth_frame.get_distance(int(marker[0][0]), int(marker[0][1]))
                dist_from_rs *= 1000

                #print('marker:{} dist_rs:{}'.format(marker, dist_from_rs))
                # cv2.putText(color_image, "%4.0fmm" % dist,
                #             (color_image.shape[1] - 500, color_image.shape[0] - 60),
                #             cv2.FONT_HERSHEY_SIMPLEX,
                #             1.2, (0, 255, 0), 3)

                cv2.putText(color_image, "%4dmm" % dist_from_rs,
                            (color_image.shape[1] - 500, color_image.shape[0] - 20),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            1.2, (0, 255, 255), 3)

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
            elif key == 0x20:
                cv2.waitKey(0)
    finally:
        cv2.destroyAllWindows()
        # Stop streaming
        pipeline.stop()


if __name__ == '__main__':
    main()
