# readme

source code refers to:
https://github.com/IntelRealSense/librealsense/blob/master/doc/stepbystep/getting_started_with_openCV.md


## opencv path

/usr/local/lib/libopencv_highgui.so.3.4
/opt/intel/computer_vision_sdk_2018.2.319/opencv/lib/libopencv_highgui.so.3.4

## using rs2 api to record/playback

https://github.com/IntelRealSense/librealsense/blob/master/src/media/readme.md

rs2::recorder
rs2::playback
rs2::config
rs2::pipeline

* it will save as ROS bag file format

## questions and todo

* how to get a frame to fetch depth from raw data?

distance: 1.351 meter will be 0x1351 in hex

## opencv python examples
/home/rasmus/src/github/librealsense/wrappers/python/examples


## showrs

A simple sample code to list version of librealsense SDK, and fetch
serial number and firmware version of realsense camera (if connected).
