// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#define SLEEP_DURATION      (500*1000)

#ifdef USE_REALSENSE

#include "cvutil.h"
#include "readsetting.h"
#include "dist.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <vector>

#include <math.h>
#include <unistd.h>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

vector<cv::Vec8i> results;
vector<cv::Scalar> colors;

inline int my_avg(int m, int n)
{
    return (m+n)/2;
}


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

void show_answer(cv::Mat& img, const cv::Vec8i& z, const cv::Scalar& color, int idx)
{
    using namespace cv;
    char msg[128];

    line(img, Point(z[0], z[1]), Point(z[2], z[3]), color, 2, LINE_AA);
    sprintf(msg, "ans(%d): %3d,%3d - %3d,%3d, [%4d], avg[%4d], med[%4d], deg:%03d",
            (int)idx, z[0], z[1], z[2], z[3], z[5], z[6], z[7], z[4]);

    rectangle(img, Point(45, 45), Point(450, 80), cv::Scalar(52, 52, 52), CV_FILLED);
    cvui::printf(img, 50, 50, msg);

    circle(img, Point(my_avg(z[0], z[2]), my_avg(z[1], z[3])), 3, Scalar(0xff, 0, 0));
    printf("%s\n", msg);
}

void init_colors()
{
    // cv color is BGR
    cv::Scalar c = cv::Scalar(0, 0, 0xff);
    colors.push_back(c);
    c = cv::Scalar(0x66, 0x66, 0xff);
    colors.push_back(c);
    c = cv::Scalar(0xcc, 0xcc, 0xff);
    colors.push_back(c);
}

float get_depth_scale(rs2::device dev)
{
    // Go over the device's sensors
    for (rs2::sensor& sensor : dev.query_sensors())
    {
        // Check if the sensor if a depth sensor
        if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
        {
            return dpt.get_depth_scale();
        }
    }
    throw std::runtime_error("Device does not have a depth sensor");
}

void remove_background(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist)
{
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];

            // Check if the depth value is invalid (<=0) or greater than the threashold
            if (pixels_distance <= 0.f || pixels_distance > clipping_dist)
            {
                // Calculate the offset in other frame's buffer to current pixel
                auto offset = depth_pixel_index * other_bpp;

                // Set pixel to "background" color (0x999999)
                std::memset(&p_other_frame[offset], 0x99, other_bpp);
            }
        }
    }
}

//#define WINDOW_NAME "Viewer to get depth"
const auto depth_window = "Depth Image";
const auto rgb_window = "RGB Image";
const auto edged_window = "Edged";

//const auto crop_window = "crop";
bool use_dep_win = true;

void init_windows()
{
    using namespace cv;
    if (use_dep_win) {
        namedWindow(depth_window, WINDOW_AUTOSIZE);
        moveWindow(depth_window, 0, 0);
    }

    //namedWindow(rgb_window, WINDOW_AUTOSIZE);
    //moveWindow(rgb_window, DEFAULT_WIDTH+55, 0);

#if 0
    namedWindow(edged_window);
    moveWindow(edged_window, 0, 0);
#endif

    // Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
    //cvui::init(depth_window);
    cvui::init(depth_window);

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

    //cv::imshow(rgb_window, cv_img);

    return true;
}

bool save_depth_to_bin(const std::string& fn, void* buffer, size_t width, size_t height)
{
    using namespace std;
    FILE* fp = fopen(fn.c_str(), "wb");
    if (fp == NULL) {
        cout << "cannot write: " << fn << endl;
        return false;
    }
    fwrite(buffer, 1, (sizeof(uint16_t) * width * height), fp);
    fclose(fp);
    return true;
}

int get_dpeth_pt(const void* buffer, int x, int y)
{
    int res = 0;
    if ( (x < 0 || x >= DEFAULT_WIDTH) || (y < 0 || y >= DEFAULT_HEIGHT) ) {
        //printf("%s: OOB %d,%d\n", __func__, x, y);
        return 0;
    }

    uint16_t* pt = (uint16_t*)buffer;
    int cnt = y * DEFAULT_WIDTH + x;
    res = (int)*(pt+cnt);

    if (false)
        printf("res: %04x\n", res);

    return res;
}


/// [in] depth_data: raw data buffer of depth
/// [in] vector to store points
/// [out] all depth values for intput points
void get_all_depth_data(const void* depth_data, const std::vector<cv::Point>& points, std::vector<int>& all_depth_results)
{
    all_depth_results.clear();
    for (auto i=0; i<points.size(); ++i)  {
        int res = get_dpeth_pt(depth_data, points.at(i).x, points.at(i).y);
        if (res) {
            all_depth_results.push_back(res);
        }
    }
}


/// [in] depth_data: raw data buffer of depth
/// [in] vector to store points
int get_avg_depth_from_points(const std::vector<int>& all_depth_results)
{
    int sum = 0;
    for (auto ii=0; ii<all_depth_results.size(); ++ii) {
        sum += all_depth_results.at(ii);
    }

    return sum / all_depth_results.size();
}

int get_median_depth_from_points(const std::vector<int>& all_depth_results)
{
    int ans = 0;
    std::vector<int> v = all_depth_results;
    std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
    auto id1 = v.size() / 2;
    if ( all_depth_results.size() % 2 ) {   // size is odd
        ans = v[id1];
    } else {    // size is even
        if (v.size() / 2 >= 1) {
            auto id0 = v.size() / 2 - 1;
            ans = (v[id0] + v[id1]) / 2;
        }
    }

    return ans;
}

bool check_point(int x1, int y1, int x2, int y2, double& degree)
{
    // area #1
    if (y1<100 && y2<100)
        return false;
    if (y1>380 && y2>380)
        return false;
    if (x1<100 && x2<100)
        return false;
    if (x1>540 && x2>540)
        return false;

    if (x1 == x2) {
        return false;
    }

    float dy = fabs(y1 - y2);
    float dx = fabs(x1 - x2);
    if (dx < 1.0 || dy < 1.0) {
        //cout << "x";
        return false;
    }

    const double PI = 3.141592653589793238462643383279502884;
    float slope = dy / dx;
    degree = atan(slope) * 180.0 / PI;
    cout << "dx " << dx << " dy " << dy << " slope: " << slope
         << " theta: " << degree << endl;
    if (slope > 1.732) {
        return false;
    }

    return true;
}

float get_length_from_vec4i(const cv::Vec4i& v)
{
    int x1 = v[0], y1 = v[1], x2 = v[2], y2 = v[3];

    float ans = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return ans;
}

bool cmp_by_length(cv::Vec8i& m, cv::Vec8i& n)
{
    return m[4] > n[4];
}

bool cmp_by_depth(cv::Vec8i& m, cv::Vec8i& n)
{
    return m[5] < n[5];
}

bool cmp_by_avg_depth(cv::Vec8i& m, cv::Vec8i& n)
{
    return m[6] < n[6];
}

bool cmp_by_med_depth(cv::Vec8i& m, cv::Vec8i& n)
{
    return m[7] < n[7];
}

void preprocess_for_edge(cv::Mat& input, cv::Mat& output)
{
    ReadSetting* settings = ReadSetting::getInstance();

    Canny(input, output, settings->canny_threshold_min, settings->canny_threshold_max);
}

///
/// [in]  color_image
/// [in]  depth_data
/// [out] edge_image
/// [out] p_lines
///
void find_edge(cv::Mat& color_image, const void* depth_data, cv::Mat& edge_image, vector<cv::Vec4i>& p_lines)
{
    using namespace cv;

    ReadSetting* settings = ReadSetting::getInstance();
    Mat gray_image;
    cvtColor(color_image, gray_image, COLOR_BGR2GRAY);
    GaussianBlur(gray_image, gray_image, Size(settings->blur_radius, settings->blur_radius), 0, 0, BORDER_DEFAULT);

    //imshow(edged_window, gray_image);

    Mat canny_output;
    preprocess_for_edge(gray_image, canny_output);

#if 0
    // find contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    Scalar color = Scalar(255, 255, 0);
    /// Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ ) {
        drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
    }
    imshow(edged_window, drawing);
#endif

    //vector<Vec4i> p_lines;
    //Mat probabilistic_hough;
    cvtColor(canny_output, edge_image, COLOR_GRAY2BGR);

    results.clear();

    /// Use Probabilistic Hough Transform
    /// 5th: votes to pass
    /// 6th: minimum line length to pass
    /// 7th: max point to jump over
    HoughLinesP(canny_output, p_lines, 1, CV_PI/180,
                settings->hough_threshold, settings->hough_minlength, settings->hough_maxlinegap);

    //Scalar color = Scalar(0xff, 0x66, 0xff);

    /// Show the result
    for( size_t i = 0; i < p_lines.size(); i++ ) {
        Vec4i l = p_lines[i];
#if 0
        Scalar color;
        if ( check_point(l[0], l[1], l[2], l[3]) ) {
            color = Scalar(0, 0, 255);
        } else {
            color = Scalar(255, 0, 0);
        }
        line( probabilistic_hough, Point(l[0], l[1]), Point(l[2], l[3]), color, 3, LINE_AA);
#endif


        // Vec6f means a vector with 6 floating numbers
        // 0:x1, y1, x2, y2,
        // 4:pt distance
        // 5:depth data at center
        // 6: average depth data
        // 7: median depth data
        double degree = 0.0;
        if (check_point(l[0], l[1], l[2], l[3], degree)) {
            Vec8i r;
            r[0] = l[0];  r[1] = l[1];  r[2] = l[2];  r[3] = l[3];
            //r[4] = get_length_from_vec4i(l);
            r[4] = degree;

            // get points from two end points
            Point p1 = Point(r[0], r[1]);
            Point p2 = Point(r[2], r[3]);
            std::vector<cv::Point> points;
            std::vector<int> all_depth_results;
            int ret = cvutil::get_points_between_two_points(canny_output, p1, p2, points);
            if (ret) {
                vector<int> all_depth_results;
                get_all_depth_data(depth_data, points, all_depth_results);
                int avg = get_avg_depth_from_points(all_depth_results);
                r[6] = avg;
                int median = get_median_depth_from_points(all_depth_results);
                r[7] = median;
            } else {
                printf("1:");
            }

            // get depth from center of line
            int tmp = get_dpeth_pt(depth_data, r[0]+r[2]/2, r[1]+r[3]/2);
            if (tmp != 0) {
                r[5] = tmp;
                results.push_back(r);
            }
            else {
                printf("0:");
            }
        }/* else {
            printf(".");
        }*/
    }

    //printf("size of results: %d ===>", (int)results.size());

    size_t num_to_show = 3;
#if 0
    // find the longest line and show
    sort(results.begin(), results.end(), cmp_by_length);
#else
    // find the nearest line and show
    //sort(results.begin(), results.end(), cmp_by_depth);
    sort(results.begin(), results.end(), cmp_by_med_depth);
#endif
    size_t results_size = results.size();


    // show answers
    for (size_t ii = 0; ii < std::min(results_size, num_to_show); ii++) {
        //printf("size of results: %d\t", (int)results.size());
        Vec8i z = results.at(ii);
        //cout << z << endl;
        show_answer(edge_image, z, colors.at(ii % colors.size()), ii);
        //line( edge_image, Point(l[0], l[1]), Point(l[2], l[3]), color, 3, LINE_AA);
    }
    // else {
    //     printf("answer: no answer...\n");
    // }
    printf("----------\n");
}

void draw_crosshair(cv::Mat& img, const cv::Point& pt)
{
    using namespace cv;
    Scalar color = Scalar(0, 0, 0xff);
    circle(img, pt, 8, color);
    Point p1 = Point(pt.x - 8, pt.y - 8);
    Point p2 = Point(pt.x + 8, pt.y + 8);
    line(img, p1, p2, color, 1, LINE_AA);
    Point p3 = Point(pt.x + 8, pt.y - 8);
    Point p4 = Point(pt.x - 8, pt.y + 8);
    line(img, p3, p4, color, 1, LINE_AA);
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

    init_colors();

    //Create a configuration for configuring the pipeline with a non default profile
    rs2::config cfg;
    //Add desired streams to configuration
    cfg.enable_stream(RS2_STREAM_COLOR, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_Z16);

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    float depth_scale = 0.0;

    if (!settings->apply_sleep) {
        // Start streaming with default recommended configuration
        rs2::pipeline_profile profile = pipe.start(cfg);
        depth_scale = get_depth_scale(profile.get_device());
        cout << "depth scale:" << depth_scale << endl;
    } else {
        std::cout << "pipe is sleeping..." << std::endl;
    }

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

    Point cross;

    while (true) {
        int64 e1 = cv::getTickCount();

        if (!settings->apply_sleep) {
            rs2::frameset frameset = pipe.wait_for_frames(); // Wait for next set of frames from the camera

            if (settings->apply_align) {
                frameset = frameset.apply_filter(align_to);
            }
            if (settings->apply_disparity) {
                frameset = frameset.apply_filter(depth2disparity);
                if (settings->apply_dec) {
                    frameset = frameset.apply_filter(dec);
                }
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

#if 1
            if (settings->distance_limit > 0.0) {
                remove_background(color, depth, depth_scale, settings->distance_limit);
            }
#endif
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

            // Create OpenCV matrix of size (w,h) from the colorized depth data
            Mat depth_image(Size(w, h), CV_8UC3, (void*)colorized_depth.get_data());
            Mat color_image(Size(w, h), CV_8UC3, (void*)color.get_data());
            Mat edge_image;

            if (settings->find_edge) {
/// try to find edge of table ... {
                vector<Vec4i> p_lines;
                find_edge(depth_image, depth.get_data(), edge_image, p_lines);
                //cvui::printf(depth_image, 180, 10, 0.6, 0xffffff, "p size: %d", p_lines.size());
/// try to find edge of table ... }
            }

/// to show depth data with mouse pointer
#if 1
            // cv::Point pt1(230, 40);
            // cv::Point pt2(450, 90);
            // cv::rectangle(depth_image, pt1, pt2, cv::Scalar(49, 52, 49), CV_FILLED);
            int x = cvui::mouse().x % DEFAULT_WIDTH;
            int y = cvui::mouse().y % DEFAULT_HEIGHT;
            cvui::printf(depth_image, 180, 30, 0.6, 0xffff00, "Mouse pointer is at (%d,%d)", x, y);

            int _depth_pt = get_dpeth_pt(depth.get_data(), x, y);
            cvui::printf(depth_image, 180, 50, 0.6, 0xffff00, "Depth is %4d (mm)", _depth_pt, _depth_pt);

            cv::Rect rectangle(0, 0, 1280, 480);
            int status = cvui::iarea(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
            switch (status) {
                case cvui::CLICK:
                    //std::cout << "Rectangle was clicked!" << std::endl;
                    //cout << "mouse click at " << x << "," << y << endl;
                    cross.x = x;
                    cross.y = y;
                    break;
                //case cvui::DOWN:
                    //cvui::printf(frame, 240, 70, "Mouse is: DOWN");
                    //cout << "mouse down " << x << "," << y << endl;
                    //break;
                default:
                    break;
            }
            //float dist = depth.get_distance(x, y);
            //cvui::printf(depth_image, 240, 90, "dist is %f", dist);
            cvui::update();
            draw_crosshair(depth_image, cross);
            draw_crosshair(color_image, cross);
#endif


            int64 e2 = cv::getTickCount();
            double elapse_time = (e2 - e1) / cv::getTickFrequency();
            //double time = cv::getTickFrequency() / (e2 - e1);
            if (settings->show_fps) {
                show_cvfps(color_image, elapse_time);
                //printf("fps: %.3f\n", elapse_time);
            }

            Mat combined;
            if (settings->find_edge) {
                hconcat(edge_image, depth_image, combined);
                hconcat(combined, color_image, combined);
            } else {
                hconcat(depth_image, color_image, combined);
            }

            //imshow(rgb_window, combined);
            imshow(depth_window, combined);

            int key = waitKey(1);
            if (key == 0x1B) {
                cout << "break" << endl;
                break;
            } else if (key == 's') {
                cout << "color: " << color_image.cols << "," << color_image.rows << endl
                    /* << " size: " << color_image.size << endl */ ;
                cout << "depth: " << depth_image.cols << "," << depth_image.rows << endl
                    /* << " size: " << depth_image.size << endl */ ;
                string fn;
                int serial = cvutil::get_timeepoch();

                fn = cvutil::compose_image_fn("color", serial);
                cout << "output color file to: " << fn << endl;
                cvutil::save_mat_to_file(color_image, fn);

                fn = cvutil::compose_image_fn("depth", serial);
                cout << "output depth file to: " << fn << endl;
                cvutil::save_mat_to_file(depth_image, fn);

                if (settings->find_edge) {
                    fn = cvutil::compose_image_fn("edged", serial);
                    cvutil::save_mat_to_file(edge_image, fn);
                }

                fn = cvutil::compose_depth_bin("depth", serial);
                save_depth_to_bin(fn, (void*)depth.get_data(), depth_image.cols, depth_image.rows);
                cout << "output depth bin to: " << fn << endl;
            }
        } else {
            usleep(SLEEP_DURATION);
        }

    }

    if (!settings->apply_sleep) {
        pipe.stop();
    }

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
