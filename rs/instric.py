#!/usr/bin/env python3

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
print("Depth Scale is: " , depth_scale)

color_stream = profile.get_stream(rs.stream.color)
depth_stream = profile.get_stream(rs.stream.depth)
ext = depth_stream.get_extrinsics_to(color_stream)
print(ext)

pipeline.stop()
