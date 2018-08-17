#!/usr/bin/env python3

'''
Read bag from file
refer from librealsense python examples for reading bag file and
another way to show depth with color map
'''



from putil import add_local_lib
add_local_lib()
# First import library
import pyrealsense2 as rs
# Import Numpy for easy array manipulation
import numpy as np
# Import OpenCV for easy image rendering
import cv2
# Import argparse for command-line options
import argparse
# Import os.path for file path manipulation
import os.path

# Create object for parsing command-line options
parser = argparse.ArgumentParser(description="Read recorded bag file and display depth stream in jet colormap.\
                                Remember to change the stream resolution, fps and format to match the recorded.")
# Add argument which takes path to a bag file as an input
parser.add_argument("-i", "--input", type=str, help="Path to the bag file")
# Parse the command line arguments to an object
args = parser.parse_args()
# Safety if no parameter have been given
if not args.input:
    print("No input paramater have been given.")
    print("For help type --help")
    exit()
# Check if the given file have bag extension
if os.path.splitext(args.input)[1] != ".bag":
    print("The given file is not of correct file format.")
    print("Only .bag files are accepted")
    exit()
try:
    # Create pipeline
    pipeline = rs.pipeline()

    # Create a config object
    config = rs.config()
    # Tell config that we will use a recorded device from filem to be used by the pipeline through playback.
    rs.config.enable_device_from_file(config, args.input)
    # Configure the pipeline to stream the depth stream
    config.enable_stream(rs.stream.depth, 1280, 720, rs.format.z16, 30)

    # Start streaming from file
    pipeline.start(config)

    # Create opencv window to render image in
    cv2.namedWindow("Depth Stream", cv2.WINDOW_AUTOSIZE)

    POIX = 640
    POIY = 480

    # Streaming loop
    while True:
        # Get frameset of depth
        frames = pipeline.wait_for_frames()

        # Get depth frame
        depth_frame = frames.get_depth_frame()
        if not depth_frame:
            continue

        # Convert images to numpy arrays
        depth_image = np.asanyarray(depth_frame.get_data())
        #color_image = np.asanyarray(color_frame.get_data())
        # Apply colormap on depth image (image must be converted to 8-bit per pixel first)
        depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.03), cv2.COLORMAP_JET)
        # Stack both images horizontally


        #width = int(depth_frame.get_width())
        #height = int(depth_frame.get_height())
        #print('depth_frame h:{} w:{}'.format(width, height))

        pois = [(POIX, POIY), (POIX, 100), (710, 200), (720, 500), (535, 200),
                (535, 500), (535, 720-132)]
        #dists = []
        for pp in pois:
            dist = depth_frame.get_distance(pp[0], pp[1])
            dist_str = '{:.0f}'.format(dist*1000)
            #dists.append(dist_str)

        # Colorize depth frame to jet colormap
        #depth_color_frame = rs.colorizer().colorize(depth_frame)
        # Convert depth_frame to numpy array to render image in opencv
        #depth_color_image = np.asanyarray(color_frame.get_data())

            cv2.circle(depth_colormap, pp, 5, color=(255, 255, 255), thickness=3)
            cv2.putText(depth_colormap, dist_str, pp, cv2.FONT_HERSHEY_SIMPLEX, 1,
                        (255, 255, 255), 2, cv2.LINE_AA)

        #images = np.hstack((color_image, depth_colormap))

        # Render image in opencv window
        cv2.imshow("Depth Stream", depth_colormap)
        # if pressed escape exit program
        if cv2.waitKey(1) == 27:
            cv2.destroyAllWindows()
            break

finally:
    pass
