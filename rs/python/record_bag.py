#!/usr/bin/env python3

#from putil import add_local_lib
import pyrealsense2 as rs
import numpy as np
import cv2
import argparse

#add_local_lib()
parser = argparse.ArgumentParser(description='simple tool to record realsense video RGB+depth')
parser.add_argument('-o', '--output', type=str, help='specify output filename', default='tmp.bag')
args = parser.parse_args()

if not args.output:
    print('use --help')
    exit()

print('will output to: {}'.format(args.output))

DEFAULT_WIDTH = 424
DEFAULT_HEIGHT = 240

try:
    pipeline = rs.pipeline()
    config = rs.config()
    config.enable_stream(rs.stream.color, DEFAULT_WIDTH, DEFAULT_HEIGHT, rs.format.bgr8, 30)
    config.enable_stream(rs.stream.depth, DEFAULT_WIDTH, DEFAULT_HEIGHT, rs.format.z16, 30)
    config.enable_record_to_file(args.output)

    profile = pipeline.start(config)
    #color_stream = profile.get_stream(rs.stream.color)
    #depth_stream = profile.get_stream(rs.stream.depth)


    while True:
        def get_depth_at_point(u, v):
            d = depth_frame.get_distance(int(u), int(v))
            #p = rs.rs2_deproject_pixel_to_point(depth_intrinsics, [u, v], d)
            return d

        frames = pipeline.wait_for_frames()
        depth_frame = frames.get_depth_frame()
        color_frame = frames.get_color_frame()

        color_image = np.asanyarray(color_frame.get_data())
        #color_image = cv2.cvtColor(color_image, cv2.COLOR_BGR2RGB)

        cv2.imshow('color', color_image)

        dd = get_depth_at_point(DEFAULT_WIDTH/2, DEFAULT_HEIGHT/2)
        print('depth: {}'.format(dd))

        key = cv2.waitKey(1)
        if key == 27:
            cv2.destroyAllWindows()
            break

finally:
    print('pipeline.stop')
    pipeline.stop()
    pass
