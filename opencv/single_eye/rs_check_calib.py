#!/usr/bin/python3
# -*- coding: utf-8 -*-

''' using realsense to calibrate checker '''

#import the necessary packages
#from glob import glob
#import math
import cv2
import numpy as np
import pyrealsense2 as rs

import checkerutil as ck

# pylint: disable=too-many-locals
# pylint: disable=too-many-statements

# checker board size 7 by 9
SIZE = (7, 9)

# size that restored from distortion
# need to be 7x, 9x
DST_W = 210
DST_H = 270

#BAG_FILE = '/home/rasmus/Documents/checker.bag'
BAG_FILE = '/home/rasmus/Documents/bucket.bag'


def get_poi_rs_dist(fourc, depth_frame, depth_intrinsics):
    ''' get_poi_rs_dist '''
    fourxyz = []
    for cc in fourc:
        #z = depth_frame.get_distance(cc[0], cc[1])
        #z *= 1000
        #print("(u, v, z) = ({}, {}, {:.2f})".format(cc[0], cc[1], z))
        u = cc[0]
        v = cc[1]
        p = uv_to_xyz(u, v, depth_frame, depth_intrinsics)
        q = ck.getmm(p)    # q is (x,y,z) in mm
        #print("(x,y,z) = ({:.2f},{:.2f},{:.2f}".format(p[0], p[1], [2]))
        #print("{:.2f},{:.2f},{:.2f}".format(q[0],q[1],q[2]))
        fourxyz.append(q)
    '''
    msg0 = "[0-1] uv({:.2f}) xyz({:.2f})".format(px_dist(fourc[0], fourc[1]), px_3d_dist(fourxyz[0], fourxyz[1]))
    msg1 = "[1-2] uv({:.2f}) xyz({:.2f})".format(px_dist(fourc[1], fourc[2]), px_3d_dist(fourxyz[1], fourxyz[2]))
    print(msg0)
    print(msg1)
    '''
    return fourxyz


# u, v is 2D pixel coordinate
# return p is 3D (x, y, z)
def uv_to_xyz(u, v, depth_frame, depth_intrinsics):
    ''' uv to xyz '''
    d = depth_frame.get_distance(u, v)
    p = rs.rs2_deproject_pixel_to_point(depth_intrinsics, [u, v], d)
    return p


def main():
    ''' main '''
    window_name = 'capture'
    result_name = 'perspective'
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
        cv2.namedWindow(result_name, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(result_name, 640, 0)

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
            #depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.03),
            #                                   cv2.COLORMAP_JET)

            # Grab new intrinsics (may be changed by decimation)
            depth_intrinsics = rs.video_stream_profile(depth_frame.profile).get_intrinsics()


            original = color_image.copy()
            color_image = cv2.cvtColor(color_image, cv2.COLOR_RGB2BGR)
            ret, crnrs = cv2.findChessboardCorners(color_image, SIZE, cv2.CALIB_CB_FAST_CHECK)
            #cv2.drawChessboardCorners(color_image, SIZE, crnrs, ret)

            # draw point of corners
            if ret:
                fourc = ck.get_poi(crnrs)
                ck.draw_poi(color_image, fourc)
                fourxyz = get_poi_rs_dist(fourc, depth_frame, depth_intrinsics)
                pp = ck.px_center(fourc[0], fourc[1], fourc[2])
                dd = depth_frame.get_distance(pp[0], pp[1])
                msg0 = "p: {} {:.0f}".format(pp, dd*1000)
                ck.put_text2(color_image, msg0, pp)
                msg0 = "H: {:.0f}px {:.0f}mm V: {:.0f}px {:.0f}mm".format(
                    ck.px_dist(fourc[0], fourc[1]),
                    ck.px_3d_dist(fourxyz[0], fourxyz[1]),
                    ck.px_dist(fourc[1], fourc[2]),
                    ck.px_3d_dist(fourxyz[1], fourxyz[2]))
                ck.put_mytext(color_image, msg0)

            # perfrom warp perspective distortion
            if ret:
                cppts = ck.get_ppts(crnrs)
                M = ck.get_perspective_mat(cppts, DST_W, DST_H)
                perspective = cv2.warpPerspective(color_image, M, (DST_W, DST_H), cv2.INTER_LINEAR)
                cv2.imshow(result_name, perspective)


            if False and ret:
                p0 = crnrs[0][0]
                p1 = crnrs[1][0]
                print("p0:{} p1:{}".format(p0, p1))
                #dist_from_rs = depth_frame.get_distance(int(marker[0][0]), int(marker[0][1]))
                dist_from_rs = depth_frame.get_distance(p0[0], p0[1])
                dist_from_rs *= 1000

                #print('marker:{} dist_rs:{}'.format(marker, dist_from_rs))
                # cv2.putText(color_image, "%4.0fmm" % dist,
                #             (color_image.shape[1] - 500, color_image.shape[0] - 60),
                #             cv2.FONT_HERSHEY_SIMPLEX,
                #             1.2, (0, 255, 0), 3)

                cv2.putText(color_image, "%4dmm" % dist_from_rs,
                            (color_image.shape[1] - 500, color_image.shape[0] - 20),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            1.1, (0, 255, 255), 3)

            # show a frame
            cv2.imshow(window_name, color_image)
            key = cv2.waitKey(1)
            if key & 0xFF == ord('q') or key == 0x1B:
                break
            if key & 0xFF == ord('s'):
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
