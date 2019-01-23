// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#define USE_UIPRESENT

#define SLEEP_DURATION      (500*1000)

#ifdef USE_REALSENSE
#define USE_GET_3DANGLE

#include "readsetting.h"
#include "cvutil.h"
#include "rsutil.h"

#ifdef USE_UIPRESENT
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#endif

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

#if 0
#define show_line() \
    printf("@line %04d\n", __LINE__);
#else
#define show_line()
#endif

// all kinds of values will push into this data structure
// Vec10i means a vector with 10 numbers
// 0,1: x1, y1, #8 is its depth
// 2,3: x2, y2, #9 is its depth
// 4: 2D degree
// 5: depth data at center
// 6: average depth data/
// 7: median depth data
///
// 8: lhs depth
// 9: rhs depth
vector<Vec10i> results;

inline int my_avg(int m, int n)
{
    return (m+n)/2;
}


#ifdef USE_UIPRESENT
// show UI version
void show_answer(cv::Mat& depth_img, cv::Mat& color_img, const Vec10i& z, const cv::Scalar& color, int idx)
{
    using namespace cv;

    if (depth_img.total() == 0)
        cout << "[WARNING] depth_img is 0\n";
    if (color_img.total() == 0)
        cout << "[WARNING] color_img is 0\n";

    char msg[128];

    line(depth_img, Point(z[0], z[1]), Point(z[2], z[3]), color, 2, LINE_AA);
    line(color_img, Point(z[0], z[1]), Point(z[2], z[3]), color, 2, LINE_AA);

    sprintf(msg, "[%d]: %3d,%3d,%4d - %3d,%3d,%4d, [%4d], a[%4d], m[%4d], d:%02d",
            (int)idx,
            z[0], z[1], z[8],
            z[2], z[3], z[9],
            z[5], z[6], z[7], z[4]);

    rectangle(depth_img, Point(45, 45), Point(500, 80), cv::Scalar(52, 52, 52), cv::FILLED);
    cvui::printf(depth_img, 50, 50, msg);

    circle(depth_img, Point(my_avg(z[0], z[2]), my_avg(z[1], z[3])), 3, Scalar(0xff, 0, 0));
    circle(color_img, Point(my_avg(z[0], z[2]), my_avg(z[1], z[3])), 3, Scalar(0xff, 0, 0));
    printf("%s\n", msg);
}
#else
// stdout version
void show_answer(cv::Mat& depth_img, cv::Mat& color_img, const Vec10i& z, const cv::Scalar& color, int idx)
{
    char msg[128];
    sprintf(msg, "[%d]: %3d,%3d,%4d - %3d,%3d,%4d, [%4d], a[%4d], m[%4d], d:%02d",
            (int)idx,
            z[0], z[1], z[8],
            z[2], z[3], z[9],
            z[5], z[6], z[7], z[4]);
    printf("%s\n", msg);
}
#endif  // USE_UIPRESENT


#ifdef USE_UIPRESENT
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

#if 0
    namedWindow(rgb_window, WINDOW_AUTOSIZE);
    moveWindow(rgb_window, DEFAULT_WIDTH+55, 0);
    namedWindow(edged_window);
    moveWindow(edged_window, 0, 0);
    namedWindow(crop_window, WINDOW_AUTOSIZE);
    moveWindow(crop_window, 0, DEFAULT_HEIGHT+50);
#endif

    // Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
    cvui::init(depth_window);
}
#endif  // USE_UIPRESENT

#ifdef USE_UIPRESENT
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
          CV_RGB(255, 255, 255), cv::FILLED);
    cv::putText(cv_img, buffer, cv::Point(0, text.height + baseline / 2.),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);

    //cv::imshow(rgb_window, cv_img);

    return true;
}
#endif  // USE_UIPRESENT

bool load_bin_to_buffer(const std::string& fn, uint8_t* buffer, size_t buffer_size)
{
    FILE* fptr = fopen(fn.c_str(), "rb");
    if (fptr == NULL) {
        return false;
    }
    size_t rs = fread(buffer, sizeof(uint8_t), buffer_size, fptr);
    //printf("rs: %d\n", (int)rs);
    (void)rs;
    fclose(fptr);
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
#if 0
int get_dpeth_pt1(const void* buffer, int x, int y)
{
    const int size_dx = 9;
    int dx[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    int tmp = get_dpeth_pt0(buffer, x, y);
    if (tmp == 0) {
        printf("retry depth...  ");
        // try to get depth from around
        int sum = 0;
        int cnt = 0;
        for (int i = 0; i < size_dx; i++) {
            int _d = get_dpeth_pt(buffer, x+dx[i], y+dy[i]);
            if (_d) {
                cnt ++;
                sum += _d;
            }
        }
        if (cnt == 0) {
            return 0;
        }
        return sum/cnt;
    }

    return tmp;
}
#endif
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

float get_length_from_vec4i(const cv::Vec4i& v)
{
    int x1 = v[0], y1 = v[1], x2 = v[2], y2 = v[3];

    float ans = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return ans;
}

bool cmp_by_length(Vec10i& m, Vec10i& n)
{
    return m[4] > n[4];
}

bool cmp_by_depth(Vec10i& m, Vec10i& n)
{
    return m[5] < n[5];
}

bool cmp_by_avg_depth(Vec10i& m, Vec10i& n)
{
    return m[6] < n[6];
}

bool cmp_by_med_depth(Vec10i& m, Vec10i& n)
{
    return m[7] < n[7];
}

void preprocess_for_edge(cv::Mat& input, cv::Mat& output)
{
    using namespace cv;

    ReadSetting* sett = ReadSetting::getInstance();
    Canny(input, output, sett->canny_threshold_min, sett->canny_threshold_max);
}

void save_procedure(const cv::Mat& color_image, const cv::Mat& depth_image, const cv::Mat& edge_image, const void* depth_data, bool isFromImage=false)
{
    ReadSetting* sett = ReadSetting::getInstance();
    string fn;
    int serial = cvutil::get_timeepoch();

    if (!isFromImage) {
        fn = cvutil::compose_image_fn("color", serial);
        cout << "output color file to: " << fn << endl;
        cvutil::save_mat_to_file(color_image, fn);

        fn = cvutil::compose_image_fn("depth", serial);
        cout << "output depth file to: " << fn << endl;
        cvutil::save_mat_to_file(depth_image, fn);

        fn = cvutil::compose_depth_bin("depth", serial);
        save_depth_to_bin(fn, (void*)depth_data, depth_image.cols, depth_image.rows);
        cout << "output depth bin to: " << fn << endl;
    }

    if (sett->find_edge) {
        fn = cvutil::compose_image_fn("edged", serial);
        cvutil::save_mat_to_file(edge_image, fn);
    }
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

    //cout << __func__ << __LINE__ << endl;

    if (color_image.total() == 0) {
        std::cout << "[ERROR] color_image is size 0\n";
        return;
    }

    // cout << "find_edge: cols: " << color_image.cols << endl
    //      << "           rows: " << color_image.rows << endl;

    ReadSetting* sett = ReadSetting::getInstance();
    Mat gray_image;
    cvtColor(color_image , gray_image, COLOR_BGR2GRAY);
    Mat gauss;
    //gauss = gray_image;
    //GaussianBlur(gray_image, gauss, Size(sett->blur_radius, sett->blur_radius), 3);
    GaussianBlur(gray_image, gauss, Size(21, 21), 0, 0);

    gray_image = gauss;
    //imshow("gauss", gray_image);
    //moveWindow("gauss", 70, 670);
    //gray_image = gauss;

    Mat canny_output;

    //Canny(gray_image, canny_output, settings->canny_threshold_min, settings->canny_threshold_max);
    Canny(gray_image, canny_output, sett->canny_threshold_min, sett->canny_threshold_max);

    Mat dst;
    float ratio = 0.25;
    // resize to 25%
    resize(canny_output, dst, Size(), ratio, ratio, INTER_AREA);
    // resize to original size
    resize(dst, canny_output, Size(), 1.0/ratio, 1.0/ratio, INTER_LINEAR);

    //canny_output = gauss;   // hacks
    cvtColor(canny_output, edge_image, COLOR_GRAY2BGR);

    results.clear();

    /// Use Probabilistic Hough Transform
    /// 5th: votes to pass
    /// 6th: minimum line length to pass
    /// 7th: max point to jump over
    HoughLinesP(canny_output, p_lines, 1, CV_PI/180,
                sett->hough_threshold, sett->hough_minlength, sett->hough_maxlinegap);

    //Scalar color = Scalar(0xff, 0x66, 0xff);

    /// Show the result
    for( size_t i = 0; i < p_lines.size(); i++ ) {
        Vec4i l = p_lines[i];

        // Vec8f means a vector with 6 floating numbers
        // 0:x1, y1, x2, y2,
        // 4: degree for 2D
        // 5: depth data at center
        // 6: average depth data
        // 7: median depth data
        double degree = 0.0;
        int z1 = get_dpeth_pt(depth_data, l[0], l[1]);
        int z2 = get_dpeth_pt(depth_data, l[2], l[3]);
        if (cvutil::check_point2(l[0], l[1], z1, l[2], l[3], z2, degree)) {
            Vec10i r;
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
                int avg = cvutil::get_avg_depth_from_points(all_depth_results);
                r[6] = avg;
                int median = cvutil::get_median_depth_from_points(all_depth_results);
                r[7] = median;
            }
            // get depth from center of line
            r[8] = z1;
            r[9] = z2;
            int tmp = get_dpeth_pt(depth_data, r[0]+r[2]/2, r[1]+r[3]/2);
            if (tmp != 0) {
                r[5] = tmp;
                results.push_back(r);
            }
        }
    }

    //printf("size of results: %d ===>", (int)results.size());

    // find the longest line and show
    //sort(results.begin(), results.end(), cmp_by_length);
    // find the nearest line and show
    //sort(results.begin(), results.end(), cmp_by_depth);
    sort(results.begin(), results.end(), cmp_by_med_depth);
    size_t results_size = results.size();
    if (results_size) {
        // show answers
        for (size_t ii = 0; ii < std::min(results_size, (size_t)sett->max_show_answer); ii++) {
            //printf("size of results: %d\t", (int)results.size());
            Vec10i z = results.at(ii);
            //cout << z << endl;
            show_answer(edge_image, color_image, z, Scalar(((ii&&ii%2)?0x7f:0), (ii?0x7f:0), 0xff), ii);
            //line( edge_image, Point(l[0], l[1]), Point(l[2], l[3]), color, 3, LINE_AA);
        }
    }
}

#ifdef USE_UIPRESENT
void do_click_window(rs2::depth_frame& depth, cv::Mat& color_image, cv::Mat& depth_image)
{
    const int printx = 180;
    const int print_inc_y = 20;
    const int print_color = 0xffff00;
    const float print_size = 0.6;
    int printy = 30;

    static cv::Point cross;
    static cv::Vec3f xyz;

    int x = cvui::mouse().x % DEFAULT_WIDTH;
    int y = cvui::mouse().y % DEFAULT_HEIGHT;
    cvui::printf(depth_image, printx, printy, print_size, print_color, "Mouse pointer is at (%d,%d)", cross.x, cross.y);
    printy += print_inc_y;

    if (cross.x == 0 && cross.y == 0) {
        // do nothing
    } else {
        int _depth_pt = get_dpeth_pt(depth.get_data(), cross.x, cross.y);
        cvui::printf(depth_image, printx, printy, print_size, print_color, "Depth is %4d (mm)", _depth_pt, _depth_pt);
        printy += print_inc_y;

        rsutil::query_uv2xyz(depth, cross, xyz);
        xyz = xyz * 1000;
        cvui::printf(depth_image, printx, printy, print_size, print_color, "xyz: %.0f,%.0f,%.0f", xyz[0], xyz[1], xyz[2]);
        printy += print_inc_y;

        double dd[3];
        dd[0] = xyz[0];  dd[1] = 0.0;  dd[2] = xyz[2];
        cv::Mat v(3, 1, CV_32F, dd);
#if 0
        double t_deg = 0.0;
        if (rsutil::get_vec_deg(v, t_deg, true)) {
            cvui::printf(depth_image, printx, printy, print_size, print_color, "deg v to u: %.2f", t_deg);
            printy += print_inc_y;
        }
#endif
    }

    cv::Rect rectangle(0, 0, DEFAULT_WIDTH*2, DEFAULT_HEIGHT);
    int status = cvui::iarea(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
    switch (status) {
        case cvui::CLICK:
            cross.x = x;
            cross.y = y;
            break;
    }
    //float dist = depth.get_distance(x, y);
    //cvui::printf(depth_image, 240, 90, "dist is %f", dist);
    cvui::update();
    cvutil::draw_crosshair(depth_image, cross);
    cvutil::draw_crosshair(color_image, cross);
}
#endif  // #ifdef USE_UIPRESENT

int test_from_image()
{
//#define USE_EDGE2
    using namespace cv;

    ReadSetting* sett = ReadSetting::getInstance();
    const std::string& cfn = sett->color_image;
    const std::string& dfn = sett->depth_image;
    const std::string& datfn = sett->depth_data;
    cout << "cfn: " << cfn << endl
         << "dfn: " << dfn << endl
         << "data: " << datfn << endl;

    // UINT16, 2bytes
    const int dep_buffer_size = DEFAULT_WIDTH * DEFAULT_HEIGHT * 2;
    uint8_t dep_buffer[dep_buffer_size];

    Mat color_img;
    Mat depth_img;
    Mat edge_img;

    string edge1_win = "edge1";
    namedWindow(edge1_win);
    moveWindow(edge1_win, 0, 0);
    string dep_win = "colorized depth";
    namedWindow(dep_win);
    moveWindow(dep_win, 0, 400);
#ifdef USE_EDGE2
    string edge2_win = "edge2";
    namedWindow(edge2_win);
    moveWindow(edge2_win, 660, 0);
#endif

    color_img = imread(cfn);
    depth_img = imread(dfn);

    imshow(dep_win, depth_img);

    load_bin_to_buffer(datfn, dep_buffer, dep_buffer_size);
    vector<Vec4i> p_lines;
    cout << "from depth_img:\n";
    find_edge(depth_img, dep_buffer, edge_img, p_lines);
    imshow(edge1_win, edge_img);
#ifdef USE_EDGE2
    cout << "from color_img:\n";
    find_edge(color_img, dep_buffer, edge_img, p_lines);
    imshow(edge2_win, edge_img);
#endif
    int key = waitKey(0);
    if (key == 0x1B) {
        cout << "user break" << endl;
    } else if (key == 's') {
        save_procedure(color_img, depth_img, edge_img, dep_buffer, true);
    }

    return 0;
}

int show_image(const std::string& fn)
{
    using namespace cv;
    if (!ReadSetting::is_file_exist(fn)) {
        cout << "image not found!\n";
        return -1;
    }

    Mat img = imread(fn);
    imshow("test", img);
    waitKey(0);
    return 0;
}


// Vec8f means a vector with 6 floating numbers
// 0:x1, y1, x2, y2,
// 4: degree for 2D
// 5: depth data at center
// 6: average depth data/
// 7: median depth data
// 8: lhs depth
// 9: rhs depth
void get_3d_angle(const rs2::depth_frame& depth)
{
    cv::Point pt1;
    cv::Vec3f xyz1;
    pt1.x = results[0][0];
    pt1.y = results[0][1];
    rsutil::query_uv2xyz(depth, pt1, xyz1);
    xyz1 *= 1000;
    printf("%s: xyz1: %.0f,%.0f,%.0f  ", __func__, xyz1[0], xyz1[1], xyz1[2]);

    cv::Point pt2;
    cv::Vec3f xyz2;
    pt2.x = results[0][2];
    pt2.y = results[0][3];
    rsutil::query_uv2xyz(depth, pt2, xyz2);
    xyz2 *= 1000;
    printf("xyz2: %.0f,%.0f,%.0f  ", xyz2[0], xyz2[1], xyz2[2]);

    double dx = xyz2[0] - xyz1[0];
    double dz = xyz2[2] - xyz1[2];

    double deg = 0.0;
    if ( cvutil::get_angle_from_dx_dy(deg, dx, dz) ) {
        printf("3D deg: %.3f\n", deg);
    } else {
        printf("\n");
    }

}


// main entry for realsense table edge finding
int test_realsense() try
{
    using namespace cv;

    ReadSetting* sett = ReadSetting::getInstance();

#ifdef USE_UIPRESENT
    init_windows();
#endif  // USE_UIPRESENT

    //Create a configuration for configuring the pipeline with a non default profile
    rs2::config cfg;
    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    float depth_scale = 0.0;

    if (sett->useBagFile()) {
        cout << "bag file: " << sett->input_bag << endl;
        cfg.enable_device_from_file(sett->input_bag);
    } else {
        if ( !rsutil::show_rsinfo() ) {
            printf("no Intel Realsense camera, exit...\n");
            exit(-1);
        }

        //Add desired streams to configuration
        cfg.enable_stream(RS2_STREAM_COLOR, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_BGR8, 15);
        cfg.enable_stream(RS2_STREAM_DEPTH, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_Z16, 15);
    }


    if (!sett->apply_sleep) {
        // Start streaming with default recommended configuration
        rs2::pipeline_profile profile = pipe.start(cfg);
        depth_scale = rsutil::get_depth_scale(profile.get_device());
        cout << "depth scale:" << depth_scale << endl;
    } else {
        std::cout << "pipe is sleeping..." << std::endl;
    }

    rs2::align align_to(RS2_STREAM_COLOR);

    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map(sett->color_scheme);    // 2: white to black

    rs2::decimation_filter dec;
    dec.set_option(RS2_OPTION_FILTER_MAGNITUDE, 2);
    rs2::disparity_transform depth2disparity;
    rs2::disparity_transform disparity2depth(false);
    rs2::spatial_filter spat;
    spat.set_option(RS2_OPTION_HOLES_FILL, 3); // 5 = fill all the zero pixels
    rs2::temporal_filter temp;
    rs2::hole_filling_filter hole;

    if (sett->show_dist) {
        std::cout << "press to continue..." << std::endl;
        waitKey(0);
    }

    rs2::frameset frameset;

    while (true) {
#ifdef USE_UIPRESENT
        int64 e1 = cv::getTickCount();
#endif
        if (!sett->apply_sleep) {
          if (!pipe.poll_for_frames(&frameset)) {
            continue;
          }
#if 0
            frameset = pipe.wait_for_frames(); // Wait for next set of frames from the camera
            if (!frameset) {
                cout << "no frameset...\n";
                continue;
            }
#endif
            if (sett->apply_align) {
                frameset = frameset.apply_filter(align_to);
            }
            if (sett->apply_disparity) {
                frameset = frameset.apply_filter(depth2disparity);
                if (sett->apply_dec) {
                    frameset = frameset.apply_filter(dec);
                }
                if (sett->apply_spatial) {
                    // Apply spatial filtering
                    frameset = frameset.apply_filter(spat);
                }
                if (sett->apply_temporal) {
                    // Apply temporal filtering
                    frameset = frameset.apply_filter(temp);
                }
                if (sett->apply_holefill) {
                    frameset = frameset.apply_filter(hole);
                }
                frameset = frameset.apply_filter(disparity2depth);
            }

            frameset = frameset.apply_filter(color_map);

            rs2::depth_frame depth = frameset.get_depth_frame();
            rs2::video_frame color = frameset.get_color_frame();

            //cout << "frame got\n";
            if (sett->distance_limit > 0.0) {
                rsutil::remove_background(color, depth, depth_scale, sett->distance_limit);
            }
            auto colorized_depth = frameset.first(RS2_STREAM_DEPTH, RS2_FORMAT_RGB8);

            // Query frame size (width and height)
            //const int w = color.as<rs2::video_frame>().get_width();
            //const int h = color.as<rs2::video_frame>().get_height();
            int w = DEFAULT_WIDTH;
            int h = DEFAULT_HEIGHT;
#if 0
            if (sett->show_dist) {
                // get distance of center point
                float dist = depth.get_distance(w/2, h/2);
                printf("from aligned depth: dist@(%d,%d) => %f\n", w/2, h/2, dist);
            }
#endif
            //cout << __LINE__ << "\n";
            // Create OpenCV matrix of size (w,h) from the colorized depth data
            Mat depth_image(Size(w, h), CV_8UC3, (void*)colorized_depth.get_data());
            Mat color_image(Size(w, h), CV_8UC3, (void*)color.get_data());
            Mat orig_depth_image = depth_image.clone();
            Mat orig_color_image = color_image.clone();
            Mat edge_image;

            //cout << __LINE__ << "\n";
            if (sett->find_edge) {
/// try to find edge of table ... {
                vector<Vec4i> p_lines;
                find_edge(depth_image, depth.get_data(), edge_image, p_lines);
                #ifdef USE_GET_3DANGLE
                // get xyz vector angle...
                if (p_lines.size()) {
                    get_3d_angle(depth);
                }
                #endif
            }
#ifdef USE_UIPRESENT
            /// to show depth data with mouse pointer
            do_click_window(depth, color_image, depth_image);

            int64 e2 = cv::getTickCount();
            double elapse_time = (e2 - e1) / cv::getTickFrequency();
            //double time = cv::getTickFrequency() / (e2 - e1);
            if (sett->show_fps) {
                show_cvfps(color_image, elapse_time);
                //printf("fps: %.3f\n", elapse_time);
            }

            Mat combined;
            if (sett->find_edge) {
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
                save_procedure(color_image, depth_image, edge_image, depth.get_data());
            } else if (key == 'c') {
                save_procedure(orig_color_image, orig_depth_image, edge_image, depth.get_data());
            }
#endif  // USE_UIPRESENT
        } else {
            usleep(SLEEP_DURATION);
        }
    }

    if (!sett->apply_sleep) {
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
