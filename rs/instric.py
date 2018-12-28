#!/usr/bin/env python3

'''
refer to:
$codbase/librealsense/wrappers/python/examples/opencv_pointcloud_viewer.py
'''

import pyrealsense2 as rs


# Create pipeline
pipeline = rs.pipeline()

# Create a config object
config = rs.config()
config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)

# Start streaming
profile = pipeline.start(config)

# Getting the depth sensor's depth scale (see rs-align example for explanation)
depth_sensor = profile.get_device().first_depth_sensor()
depth_scale = depth_sensor.get_depth_scale()
print("Depth Scale is: ", depth_scale)

color_stream = profile.get_stream(rs.stream.color)
depth_stream = profile.get_stream(rs.stream.depth)
ext = depth_stream.get_extrinsics_to(color_stream)
print(ext)

cnt = 0
while cnt < 1000:
    frames = pipeline.wait_for_frames()

    depth_frame = frames.get_depth_frame()
    color_frame = frames.get_color_frame()

    # Grab new intrinsics (may be changed by decimation)
    depth_intrinsics = rs.video_stream_profile(
        depth_frame.profile).get_intrinsics()
    w, h = depth_intrinsics.width, depth_intrinsics.height

    # u, v is 2D pixel coordinate
    # return p is 3D (x, y, z)
    def get_point(u, v):
        d = depth_frame.get_distance(u, v)
        p = rs.rs2_deproject_pixel_to_point(depth_intrinsics, [u, v], d)
        return p

    pt = get_point(320, 240)
    print("pt = {}".format(pt))
    cnt += 1

pipeline.stop()
