// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#ifdef USE_REALSENSE

#include "cvutil.h"
#include "readsetting.h"

#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

bool show_info()
{
    rs2::context ctx;
    auto devices = ctx.query_devices();
    size_t device_count = devices.size();
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

const auto depth_window = "Depth Image";
const auto rgb_window = "RGB Image";
//const auto crop_window = "crop";
bool use_dep_win = true;

void init_windows()
{
    using namespace cv;
    if (use_dep_win) {
        namedWindow(depth_window, WINDOW_AUTOSIZE);
        moveWindow(depth_window, 0, 0);
    }

    namedWindow(rgb_window, WINDOW_AUTOSIZE);
    moveWindow(rgb_window, DEFAULT_WIDTH+55, 0);

    // namedWindow(crop_window, WINDOW_AUTOSIZE);
    // moveWindow(crop_window, 0, DEFAULT_HEIGHT+50);
}

bool show_cvfps(cv::Mat& cv_img, double elapsed_time)
{
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.75;
    int baseline = 0;
    int thickness = 2;
    const int BUFFER_SIZE = 80;
    char buffer[BUFFER_SIZE];
    double fps = 1.0 / elapsed_time;

    snprintf(buffer, BUFFER_SIZE, "elapsed: %.3f fps(%.1f)", elapsed_time, fps);
    cv::Size text = cv::getTextSize(buffer, fontface, scale, thickness, &baseline);
    cv::rectangle(cv_img, cv::Point(0, 0),cv::Point(text.width, text.height + baseline),
          CV_RGB(255, 255, 255), CV_FILLED);
    cv::putText(cv_img, buffer, cv::Point(0, text.height + baseline / 2.),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);

    cv::imshow(rgb_window, cv_img);

    return true;
}

int test_realsense() try
{
    using namespace cv;

    ReadSetting* settings = ReadSetting::getInstance();

    init_windows();
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
    rs2::pipeline_profile profile = pipe.start(cfg);

    rs2::align align_to(RS2_STREAM_COLOR);

    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map(settings->color_scheme);    // 2: white to black

    rs2::decimation_filter dec;
    dec.set_option(RS2_OPTION_FILTER_MAGNITUDE, 2);
    rs2::disparity_transform depth2disparity;
    rs2::disparity_transform disparity2depth(false);
    rs2::spatial_filter spat;
    spat.set_option(RS2_OPTION_HOLES_FILL, 3); // 5 = fill all the zero pixels
    rs2::temporal_filter temp;
    rs2::hole_filling_filter hole;

    if (settings->show_dist) {
        std::cout << "press to continue..." << std::endl;
        waitKey(0);
    }


    while (true)
    {
        int64 e1 = cv::getTickCount();
        rs2::frameset frameset = pipe.wait_for_frames(); // Wait for next set of frames from the camera

        if (settings->apply_align) {
            frameset = frameset.apply_filter(align_to);
        }

        if (settings->apply_disparity) {

            frameset = frameset.apply_filter(depth2disparity);

            if (settings->apply_dec)
                frameset = frameset.apply_filter(dec);

            if (settings->apply_spatial) {
                // Apply spatial filtering
                frameset = frameset.apply_filter(spat);
            }

            if (settings->apply_temporal) {
                // Apply temporal filtering
                frameset = frameset.apply_filter(temp);
            }

            if (settings->apply_holefill) {
                frameset = frameset.apply_filter(hole);
            }

            frameset = frameset.apply_filter(disparity2depth);
        }

        frameset = frameset.apply_filter(color_map);

        rs2::depth_frame depth = frameset.get_depth_frame();
        rs2::video_frame color = frameset.get_color_frame();
        auto colorized_depth = frameset.first(RS2_STREAM_DEPTH, RS2_FORMAT_RGB8);

        // Query frame size (width and height)
        //const int w = color.as<rs2::video_frame>().get_width();
        //const int h = color.as<rs2::video_frame>().get_height();
        int w = DEFAULT_WIDTH;
        int h = DEFAULT_HEIGHT;

        if (settings->show_dist) {
            // get distance of center point
            float dist = depth.get_distance(w/2, h/2);
            printf("from aligned depth: dist@(%d,%d) => %f\n", w/2, h/2, dist);
        }


        //rs2::frame depth_color_frame = depth.apply_filter(color_map);

        // Create OpenCV matrix of size (w,h) from the colorized depth data
        Mat depth_image(Size(w, h), CV_8UC3, (void*)colorized_depth.get_data());
        Mat color_image(Size(w, h), CV_8UC3, (void*)color.get_data());
        //Mat new_img;

        int64 e2 = cv::getTickCount();
        double elapse_time = (e2 - e1) / cv::getTickFrequency();
        //double time = cv::getTickFrequency() / (e2 - e1);
        if (settings->show_fps) {
            show_cvfps(color_image, elapse_time);
            //printf("fps: %.3f\n", elapse_time);
        }


        imshow(rgb_window, color_image);
        imshow(depth_window, depth_image);

        int key = waitKey(1);
        if (key == 0x1B) {
            break;
        }
    }
    pipe.stop();
    destroyAllWindows();
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
