// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#ifdef USE_REALSENSE

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <stdio.h>
#include <iostream>

namespace pbox {

const int fontface = cv::FONT_HERSHEY_SIMPLEX;
const double scale = 1;
const int thickness = 2;
int baseline = 0;
const int DEFAULT_WIDTH = 640;
const int DEFAULT_HEIGHT = 480;

void draw_aim(cv::Mat& img, int x, int y, int w, int h)
{
    using namespace cv;

    const int radius = 30;
    const int tail = 5;
    Scalar white = Scalar(255, 255, 255);
    int cx = x + w/2;
    int cy = y + h/2;

    circle(img, Point(cx, cy), radius, white, thickness);
    line(img, Point(cx-radius-tail, cy), Point(cx+radius+tail, cy), white, thickness);
    line(img, Point(cx, cy-radius-tail), Point(cx, cy+radius+tail), white, thickness);
}

void draw_dist(cv::Mat& img, float dist)
{
    const int buffer_size = 128;
    using namespace cv;
    char buffer[buffer_size];

    snprintf(buffer, buffer_size, "dist: %.3fm", dist);
    cv::Size text = cv::getTextSize(buffer, fontface, scale, thickness, &baseline);
    cv::rectangle(img, cv::Point(0, 0),cv::Point(text.width, text.height + baseline),
          CV_RGB(255, 255, 255), CV_FILLED);
    cv::putText(img, buffer, cv::Point(0, text.height + baseline / 2.),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

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

bool isInited = false;
bool isAssigned = false;
rs2::pipeline pipe;
rs2::frameset g_data;

void init_realsense()
{
    if (isInited) {
        // do nothing and exit
    } else {
        show_info();
        rs2::config cfg;
        //Add desired streams to configuration
        cfg.enable_stream(RS2_STREAM_COLOR, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_BGR8, 30);
        cfg.enable_stream(RS2_STREAM_DEPTH, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_Z16);
        // Declare RealSense pipeline, encapsulating the actual device and sensors
        // Start streaming with default recommended configuration
        pipe.start(cfg);

        for (auto i = 0; i < 45; ++i)
            pipe.wait_for_frames();

        isInited = true;
    }
}

int get_dist_from_point(int x, int y)
{
    float _dist = 0.0;
    //printf("try to %s\n", __func__);
    // get distance of center point
    _dist = g_data.get_depth_frame().get_distance(x, y);
    _dist = _dist * 100.0;  // make it as cm
    int ret = (int)_dist;
    return ret;
}


int get_color_mat_from_realsense(cv::Mat& image)
{
    using namespace cv;

    // // wait a moment
    // for (auto i = 0; i < 30; ++i)
    //     pipe.wait_for_frames();

    g_data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
    rs2::frame color = g_data.get_color_frame();

    // Query frame size (width and height)
    const int w = color.as<rs2::video_frame>().get_width();
    const int h = color.as<rs2::video_frame>().get_height();

    //printf("%s: w=%d, h=%d\n", __func__, w, h);

    Mat color_image(Size(w, h), CV_8UC3, (void*)color.get_data());
    color_image.copyTo(image);
    return 0;
}

int test_realsense() try
{
    using namespace cv;
    const auto depth_window = "Depth Image";
    const auto rgb_window = "RGB Image";
    namedWindow(depth_window, WINDOW_AUTOSIZE);
    namedWindow(rgb_window, WINDOW_AUTOSIZE);
    moveWindow(depth_window, 0, 0);
    moveWindow(rgb_window, DEFAULT_WIDTH+50, 0);

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

    while (waitKey(1) < 0 && cvGetWindowHandle(depth_window))
    {
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
        rs2::frame depth = color_map(data.get_depth_frame());
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

        draw_aim(depth_image, 0, 0, w, h);
        draw_aim(color_image, 0, 0, w, h);
        draw_dist(depth_image, dist);

        // Update the window with new data
        imshow(depth_window, depth_image);
        imshow(rgb_window, color_image);
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


}   // namespace pbox

#endif  // USE_REALSENSE
