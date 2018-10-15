// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#ifdef USE_REALSENSE

#include "cvutil.h"
#include "readsetting.h"

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <cstdio>
#include <iostream>

bool show_info()
{
    using namespace std;

    rs2::context ctx;
    auto devices = ctx.query_devices();
    size_t device_count = devices.size();
    printf("device_count: %d\n", (int)device_count);
    for (size_t i = 0; i < device_count; ++i)
    {
        auto dev = devices[i];
        string dev_name = dev.get_info(RS2_CAMERA_INFO_NAME);
        if ( dev_name.find("Intel") != std::string::npos) {
            std::cout << "found Intel!" << '\n';
            if ( dev.get_info(RS2_CAMERA_INFO_NAME) ) {
                cout << left
                << setw(4) << "[" << i << "] "
                << setw(30) << dev.get_info(RS2_CAMERA_INFO_NAME)
                << setw(20) << dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER)
                << setw(20) << dev.get_info(RS2_CAMERA_INFO_FIRMWARE_VERSION)
                << endl;
            }
            return true;
        }
    }

    // no camera name with "Intel"
    return false;
}

int test_realsense() try
{
    using namespace cv;

    bool use_dep_win = true;

    const auto depth_window = "Depth Image";
    const auto rgb_window = "RGB Image";
    const auto crop_window = "crop";

    if (use_dep_win) {
        namedWindow(depth_window, WINDOW_AUTOSIZE);
        moveWindow(depth_window, 0, 0);
    }
    namedWindow(rgb_window, WINDOW_AUTOSIZE);
    namedWindow(crop_window, WINDOW_AUTOSIZE);

    moveWindow(rgb_window, DEFAULT_WIDTH+50, 0);
    moveWindow(crop_window, 0, DEFAULT_HEIGHT+50);

    if ( !show_info() ) {
        printf("no Intel Realsense camera, exit...\n");
        exit(-1);
    }

    //Create a configuration for configuring the pipeline with a non default profile
    rs2::config cfg;
    //Add desired streams to configuration
    cfg.enable_stream(RS2_STREAM_COLOR, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_Z16);

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start(cfg);

    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    while (waitKey(1) < 0 /*&& cvGetWindowHandle(rgb_window)*/)
    {
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
        rs2::frame depth = data.get_depth_frame();
        rs2::frame color = data.get_color_frame();

        // Query frame size (width and height)
        const int w = depth.as<rs2::video_frame>().get_width();
        const int h = depth.as<rs2::video_frame>().get_height();

        // get distance of center point
        float dist = data.get_depth_frame().get_distance(w/2, h/2);
        //printf("dist at %d,%d => %f\n", w/2, h/2, dist);

        // Create OpenCV matrix of size (w,h) from the colorized depth data
        Mat depth_image(Size(w, h), CV_8UC3, (void*)depth.get_data(), Mat::AUTO_STEP);
        Mat color_image(Size(w, h), CV_8UC3, (void*)color.get_data());
        Mat new_img;

        if (use_dep_win) {
            cvutil::draw_aim(depth_image, w, h);
            cvutil::draw_dist(depth_image, dist);
        }

        cvutil::draw_aim(color_image, w, h);
        cvutil::draw_aim(color_image, w, h);

        // Update the window with new data
        if (use_dep_win) {
            imshow(depth_window, depth_image);
        }

        imshow(rgb_window, color_image);

        const int max_repeat = 3;
        for (int ii=0; ii<max_repeat; ++ii) {
            cvutil::crop_image(color_image, new_img);
            imshow(crop_window, new_img);
        }
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}


#endif  // USE_REALSENSE
