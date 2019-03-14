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
#include <stdlib.h>
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

cv::Scalar khaki = cv::Scalar(0, 0x80, 0x80);
cv::Scalar medlavendar = cv::Scalar(0xB9, 0x6E, 0xA0);
cv::Scalar color_answer = cv::Scalar(0x6c, 0x33, 0x65);

bool g_verbose = false;

int get_dpeth_pt(const void* buffer, int x, int y);

inline int my_avg(int m, int n)
{
    return (m+n)/2;
}


class RcAnswer {
public:
    RcAnswer(const cv::Point& m, const cv::Point& n) {
        p0 = m;
        p1 = n;
        p_mid = cv::Point(my_avg(m.x, n.x), my_avg(m.y, n.y));
    }
    // 2D points
    cv::Point p0;
    cv::Point p1;
    cv::Point p_mid;
    int center_depth = 0;
    int average_depth = 0;
    int median_depth = 0;
};


class SmallDots
{
public:
    SmallDots(cv::Mat& cimg, cv::Mat& dimg, const void* buf) {
        color_image = cimg;
        depth_image = dimg;
        depth_data = buf;
    }

    void go();
    int find_small_dist();
    void find_small_dot();
    void show_small_dots();
    int find_overlap(const std::vector<cv::Point>& pol);

    float hausdorff_distance(const std::vector<cv::Point>& other) {
        return hausdorff_distance(dots, other);
    }

    bool getIsCloser() const {
        return isCloser;
    }

    static float oneway_hausdorff(const std::vector<cv::Point>& A, const std::vector<cv::Point>& B);
    static float hausdorff_distance(const std::vector<cv::Point>& A, const std::vector<cv::Point>& B);

    bool edge_from_small_dots();
    bool get_3d_angle(const rs2::depth_frame& depth, float& deg3d);

    static float dist_2d(const cv::Point& p, const cv::Point& q);
    static float find_small_value(const std::vector<float>& v);

public:
    bool pass_check = false;

    int prev_min_dist = -1;
    int64 prev_epoch = 0;
    int min_dist = -1;
    int64 curr_epoch = 0;
    bool isCloser = false;

    cv::Point small_pt;
    std::vector<cv::Point> dots;

    cv::Point p0;
    cv::Point p1;
    double degree = -1;

    cv::Mat color_image;
    cv::Mat depth_image;
    const void* depth_data = NULL;

    const size_t NUM_LIMIT = 150;
};

int SmallDots::find_overlap(const std::vector<cv::Point>& pol)
{
    int count = 0;
    for (size_t ii=0; ii<pol.size(); ++ii) {
        cv::Point foo = pol.at(ii);
        std::vector<cv::Point>::iterator it;
        it = std::find(dots.begin(), dots.end(), foo);
        if (it != dots.end()) {
            count ++;
        }
    }

    //printf("overlap: %d from %d\n", count, static_cast<int>(pol.size()));

    return count;
}

float SmallDots::oneway_hausdorff(const std::vector<cv::Point>& A, const std::vector<cv::Point>& B)
{
    float dist = 0.0;
    for (size_t ii=0; ii<A.size(); ++ii) {
        float shortest = 999999999;
        for (size_t jj=0; jj<B.size(); ++jj) {
            float d = dist_2d(A.at(ii), B.at(jj));
            if (d < shortest) {
                shortest = d;
            }
        }
        if (shortest > dist) {
            dist = shortest;
        }
    }
    return dist;
}

float SmallDots::hausdorff_distance(const std::vector<cv::Point>& A, const std::vector<cv::Point>& B)
{
    float dis_a = SmallDots::oneway_hausdorff(A, B);
    float dis_b = SmallDots::oneway_hausdorff(B, A);
    return ((dis_a > dis_b) ? dis_a : dis_b);
}

void SmallDots::go()
{
    // find mini dist
    min_dist = find_small_dist();
    curr_epoch = cv::getTickCount();

    if (curr_epoch > prev_epoch) {
        isCloser = (min_dist < prev_min_dist);
        prev_min_dist = min_dist;
        prev_epoch = curr_epoch;
    }


    // find small dots
    find_small_dot();
    show_small_dots();
    pass_check = edge_from_small_dots();
}

int SmallDots::find_small_dist()
{
    int smallest_dist = -1;

    small_pt.x = 0;
    small_pt.y = 0;

    for (int x = 0 ; x < DEFAULT_WIDTH; ++x) {
        for (int y = 0; y < DEFAULT_HEIGHT; ++y) {
            int dist = get_dpeth_pt(depth_data, x, y);
            if (dist != 0) {
                if (smallest_dist == -1 || (dist <= smallest_dist)) {
                    smallest_dist = dist;
                    small_pt.x = x;
                    small_pt.y = y;
                }
            }
        }
    }
    //printf("find_small_dist: %d @(%d,%d)\n", smallest_dist, small_pt.x, small_pt.y);
    return smallest_dist;
}

void SmallDots::find_small_dot()
{
    const int SMALL_RANGE = 5;
    const int margin = ReadSetting::getInstance()->ignore_margin;
    dots.clear();
    cv::Point pt;
    for (int x = 0 ; x < DEFAULT_WIDTH; ++x) {
        for (int y = margin; y < DEFAULT_HEIGHT-margin; ++y) {
            int dist = get_dpeth_pt(depth_data, x, y);
            if (abs(dist - min_dist) < SMALL_RANGE) {
                pt.x = x;
                pt.y = y;
                dots.push_back(pt);
            }
        }
    }
    if (g_verbose) {
        printf("find_small_dot: %d dots\n", static_cast<int>(dots.size()));
    }
}

void SmallDots::show_small_dots()
{
    char msg[80];
    sprintf(msg, "%03d (%d)pts", min_dist, static_cast<int>(dots.size()));
    cv::putText(color_image, msg, cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 0.75, khaki, 2);

    for (size_t jj=0; jj<dots.size(); ++jj) {
        cv::circle(color_image, dots[jj], 2, khaki);
        cv::circle(depth_image, dots[jj], 2, khaki);
    }
}

float SmallDots::dist_2d(const cv::Point& p, const cv::Point& q)
{
    return sqrt( pow(p.x - p.y, 2) + pow(q.x - q.y, 2) );
}

float SmallDots::find_small_value(const std::vector<float>& v)
{
#if 0
    float res = -1;
    for (size_t ii=0; ii < v.size(); ++ii) {
        if (res == -1 || v.at(ii) <= res) {
            res = v.at(ii);
        }
    }
#else
    float res = *std::min_element(v.cbegin(), v.cend());
#endif
    return res;
}

bool SmallDots::edge_from_small_dots()
{
    if (dots.size() < NUM_LIMIT) {
        //printf("drop, size: %d\n", static_cast<int>(dots.size()));
        return false;
    }
    int min_x = -1;
    int max_x = -1;
    int min_y = -1;
    int max_y = -1;
    for (size_t i=0; i<dots.size(); ++i) {
        cv::Point p = dots.at(i);
        if (min_x == -1 || p.x <= min_x) {
            min_x = p.x;
        }
        if (max_x == -1 || p.x >= max_x) {
            max_x = p.x;
        }
        if (min_y == -1 || p.y <= min_y) {
            min_y = p.y;
        }
        if (max_y == -1 || p.y >= max_y) {
            max_y = p.y;
        }
    }
    if (min_x < 0 || max_y < 0 || min_y < 0 || max_y < 0) {
        return false;
    }

    p0 = cv::Point(min_x, my_avg(min_y, max_y));
    p1 = cv::Point(max_x, my_avg(min_y, max_y));
    cv::Point m = cv::Point(my_avg(p0.x, p1.x), my_avg(p0.y, p1.y));
    int dist_m = get_dpeth_pt(depth_data, m.x, m.y);

    int d0 = get_dpeth_pt(depth_data, p0.x, p0.y);
    int d1 = get_dpeth_pt(depth_data, p1.x, p1.y);

    if (cvutil::check_point2(p0.x, p0.y, d0, p1.x, p1.y, d1, min_dist, degree)) {
        printf("2D: SMALLDOTS: minD(%d) sz(%d) Dmid(%d) deg(%.f) %d,%d-%d,%d\n",
            //dist_2d(p0, p1),    // 2d distance of p0, p1
             min_dist,           // z depth minimum
            static_cast<int>(dots.size()),
            dist_m,             // z depth of midpoint
            degree,             // deg
            min_x, min_y, max_x, max_y);
        cv::line(color_image, p0, p1, cv::Scalar(0,0,255), 2, cv::LINE_AA);
        cv::line(depth_image, p0, p1, cv::Scalar(0,0,255), 2, cv::LINE_AA);
        cv::circle(color_image, m, 2, khaki);
        return true;
    } else {
        //printf("failed to pass check\n");
        return false;
    }
}

bool SmallDots::get_3d_angle(const rs2::depth_frame& depth, float& degree3d)
{
    degree3d = INVALID_DEGREE;
    if (!depth) {
        return false;
    }
    //printf("enter %s...\n", __func__);
    cv::Point pt1;
    cv::Vec3f xyz1;
    pt1.x = p0.x;
    pt1.y = p0.y;

    cv::Point pt2;
    cv::Vec3f xyz2;
    pt2.x = p1.x;
    pt2.y = p1.y;

    bool ret1 = rsutil::query_uv2xyz(depth, pt1, xyz1);
    bool ret2 = rsutil::query_uv2xyz(depth, pt2, xyz2);

    if (ret1) {
        xyz1 *= 1000;
        //printf("%s: xyz1: %.0f,%.0f,%.0f  ", __func__, xyz1[0], xyz1[1], xyz1[2]);
    } else {
        if (g_verbose) {
            printf("ret1 failed\n");
        }
        return false;
    }
    if (ret2) {
        xyz2 *= 1000;
        //printf("xyz2: %.0f,%.0f,%.0f  ", xyz2[0], xyz2[1], xyz2[2]);
    } else {
        if (g_verbose) {
            printf("ret2 failed\n");
        }
        return false;
    }

    float dist3d = rsutil::dist_3d_xyz(xyz1, xyz2);
    printf("    dist3d: %.2f ", dist3d);

    float dx = xyz2[0] - xyz1[0];
    float dz = xyz2[2] - xyz1[2];

    bool ret = cvutil::get_angle_from_dx_dy(degree3d, dx, dz, false);
    if (ret) {
        printf("  angle: %.2f\n", degree3d);
    } else {
        printf("\n");
    }
    return ret;
}

void dd2d(const Vec11i& r, const SmallDots& sm)
{
    const float PIXEL_LIMIT = 100.0;
    // calculate distance between first line to smallest pt
    RcAnswer ans(cv::Point(r[0],r[1]), cv::Point(r[2], r[3]));
    ans.center_depth = r[5];
    ans.average_depth = r[6];
    ans.median_depth = r[7];

    std::vector<float> vv;
    float d2f = SmallDots::dist_2d(ans.p_mid, sm.small_pt);
    vv.push_back(d2f);
    d2f = SmallDots::dist_2d(cv::Point(r[0], r[1]), sm.small_pt);
    vv.push_back(d2f);
    d2f = SmallDots::dist_2d(cv::Point(r[2], r[2]), sm.small_pt);
    vv.push_back(d2f);

    float s2f = SmallDots::find_small_value(vv);

    printf("[INFO] dd2d: %.0f  *****  ", s2f);
    if (d2f > PIXEL_LIMIT) {
        printf("[WARNING] too far to small point...\n");
    }
}

// all kinds of values will push into this data structure
// Vec11i means a vector with 11 numbers
// 0,1: x1, y1, #8 is its depth
// 2,3: x2, y2, #9 is its depth
// 4: 2D degree
// 5: depth data at center
// 6: average depth data/
// 7: median depth data
///
// 8: lhs depth
// 9: rhs depth
// 10: overlapped counts
vector<Vec11i> results;
std::vector<Vec11i> trace_z;


#ifdef USE_UIPRESENT
// show UI version
void draw_answer_line(cv::Mat& img, const Vec11i& z, const cv::Scalar& color)
{
    using namespace cv;

    if (img.total() == 0) {
        cout << "[WARNING] img is invalid\n";
        return;
    } else {

        line(img, Point(z[0], z[1]), Point(z[2], z[3]), color, 1.5, LINE_AA);
        circle(img, Point(my_avg(z[0], z[2]), my_avg(z[1], z[3])), 3, Scalar(0xff, 0, 0));
    }
}

#endif  // USE_UIPRESENT

// stdout version
void print_answer_string(const Vec11i& z, int idx, int min_dist=0)
{
    char msg[128];

    if (g_verbose) {
        sprintf(msg, "[%d]:%3d,%3d, %3d-%3d,%3d, %3d, ",
            idx,
            z[0], z[1], z[8],
            z[2], z[3], z[9]);
    } else {
        sprintf(msg, "[%d]: %3d - %3d, ",
            idx, z[8], z[9]);
    }

    sprintf(msg, "%s minD<%d> [%d] a[%d] m[%d], 2D deg:%d  hausdorff:%d",
            msg, min_dist, z[5], z[6], z[7], z[4], z[10]);

    printf("%s\n", msg);
}

Vec11i handle_lastz(const Vec11i& z, const SmallDots& sm)
{
    const int slide_win = 10;
    Vec11i last_z;
    Vec11i avg_z;

    if (trace_z.size() < slide_win) {
        trace_z.push_back(z);
    }
    else {
        last_z = trace_z[trace_z.size()-1];
        if ( (abs(z[8] - last_z[8]) < DEPTH_SMALL_DIFF)
            && (abs(z[9] - last_z[9]) < DEPTH_SMALL_DIFF) ) {
            trace_z.push_back(z);
        } else {
            printf("handle_lastz: rejected\n");
        }
    }
    return avg_z;
}


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
    const double scale = 0.75;
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
        printf("%s: OOB %d,%d\n", __func__, x, y);
        return 0;
    }

    uint16_t* pt = (uint16_t*)buffer;
    int cnt = y * DEFAULT_WIDTH + x;
    res = (int)*(pt+cnt);

    //printf("res: %04d\n", res);

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
        if (rsutil::check_depth(res)) {
            all_depth_results.push_back(res);
        }
    }
    const float usable_limit = 60.0;
    if (points.empty()) {
        printf("[WARN] no usable points\n");
    } else {
        float usable_ratio = (float)all_depth_results.size()*100.0/(float)points.size();
        if (usable_ratio < usable_limit) {
            printf("[WARN] usable ratio < %.0f%%: %.0f\n", usable_limit, usable_ratio);
        }
    }
}

float get_length_from_vec4i(const cv::Vec4i& v)
{
    int x1 = v[0], y1 = v[1], x2 = v[2], y2 = v[3];

    float ans = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return ans;
}

bool cmp_by_length(Vec11i& m, Vec11i& n)
{
    return m[4] > n[4];
}

bool cmp_by_depth(Vec11i& m, Vec11i& n)
{
    return m[5] < n[5];
}

bool cmp_by_avg_depth(Vec11i& m, Vec11i& n)
{
    return m[6] < n[6];
}

bool cmp_by_med_depth(Vec11i& m, Vec11i& n)
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



cv::Mat resize_forward_and_backward(const cv::Mat& input)
{
    const float ratio = 0.25;
    cv::Mat tmp;
    cv::Mat ret;
    // resize to a smaller size
    cv::resize(input, tmp, cv::Size(), ratio, ratio, cv::INTER_AREA);
    // resize to original size
    cv::resize(tmp, ret, cv::Size(), 1.0/ratio, 1.0/ratio, cv::INTER_LINEAR);

    return ret;
}

///
/// [in]  color_image
/// [in]  depth_data
/// [out] edge_image
/// [out] p_lines
///
void find_edge(cv::Mat& color_image, const void* depth_data, cv::Mat& edge_image, vector<cv::Vec4i>& p_lines, SmallDots& sm)
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
    //GaussianBlur(gray_image, gauss, Size(21, 21), 0, 0);
    GaussianBlur(gray_image, gauss, Size(21, 21), 0, 0);

#if 1
    gray_image = resize_forward_and_backward(gauss);
#else
    gray_image = gauss;
#endif
    //imshow("gauss", gray_image);
    //moveWindow("gauss", 70, 670);
    //gray_image = gauss;

    Mat canny_output;

    if (g_verbose) {
        printf("canny_threshold ==> min: %d, max: %d\n",
            sett->canny_threshold_min, sett->canny_threshold_max);
    }
    Canny(gray_image, canny_output, sett->canny_threshold_min, sett->canny_threshold_max);

    //Mat back = resize_forward_and_backward(canny_output);

    //canny_output = gauss;   // hacks
    cvtColor(canny_output, edge_image, COLOR_GRAY2BGR);

    results.clear();

    /// Use Probabilistic Hough Transform
    /// 5th: votes to pass
    /// 6th: minimum line length to pass
    /// 7th: max point to jump over
    HoughLinesP(canny_output, p_lines, 1, CV_PI/180,
                sett->hough_threshold, sett->hough_minlength, sett->hough_maxlinegap);

    if (g_verbose) {
        printf("p_lines.size: %d, hough_threshold: %d, hough_minlength: %d, hough_maxlinegap: %d\n",
           (int)p_lines.size(), sett->hough_threshold, sett->hough_minlength, sett->hough_maxlinegap);
    }
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
        if (cvutil::check_point2(l[0], l[1], z1, l[2], l[3], z2, sm.min_dist, degree)) {
            Vec11i r;
            r[0] = l[0];  r[1] = l[1];  r[2] = l[2];  r[3] = l[3];
            //r[4] = get_length_from_vec4i(l);
            r[4] = degree;

            // get points from two end points
            Point p1 = Point(r[0], r[1]);
            Point p2 = Point(r[2], r[3]);
            std::vector<cv::Point> pol;
            std::vector<int> all_depth_results;
            int ret = cvutil::get_points_between_two_points(canny_output, p1, p2, pol);
            if (ret) {
                vector<int> all_depth_results;
                get_all_depth_data(depth_data, pol, all_depth_results);
                int avg = cvutil::get_avg_depth_from_points(all_depth_results);
                r[6] = avg;
                int median = cvutil::get_median_depth_from_points(all_depth_results);
                r[7] = median;
                r[10] = (int)sm.hausdorff_distance(pol);
            } else {
                printf("REJECT: NO POINTS OF LINE\n");
                continue;
            }
            // get depth from center of line
            r[8] = z1;
            r[9] = z2;
            int tmp = get_dpeth_pt(depth_data, (r[0]+r[2])/2, (r[1]+r[3])/2);
            if (rsutil::check_depth(tmp)) {
                r[5] = tmp;
                results.push_back(r);
                //printf("[INFO] push at %d\n", static_cast<int>(results.size()-1));
            } else {
                printf("REJECT: find_edge: invalid depth at mid pt\n");
            }
        } else {
            if (g_verbose) {
                printf("REJECT: p_lines(%d)\n", (int)i);
            }
            //printf("")
        }
    }

    ////////// sort lines //////////
    // find the longest line and show
    //sort(results.begin(), results.end(), cmp_by_length);
    // find the nearest line and show
    //sort(results.begin(), results.end(), cmp_by_depth);
    sort(results.begin(), results.end(), cmp_by_med_depth);
}

#ifdef USE_UIPRESENT
void do_click_window(rs2::depth_frame& depth, cv::Mat& color_image, cv::Mat& depth_image)
{
    const int printx = 30;
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

        bool ret = rsutil::query_uv2xyz(depth, cross, xyz);
        if (ret) {
            xyz = xyz * 1000;
            cvui::printf(depth_image, printx, printy, print_size, print_color, "xyz: %.0f,%.0f,%.0f", xyz[0], xyz[1], xyz[2]);
            printy += print_inc_y;
        }

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
    cvui::update();
    cvutil::draw_crosshair(depth_image, cross);
    cvutil::draw_crosshair(color_image, cross);
}
#endif  // #ifdef USE_UIPRESENT

int test_from_image()
{
//#define USE_EDGE2
    using namespace cv;

    g_verbose = true;

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

    string edge1_win = "edge1";
    namedWindow(edge1_win);
    moveWindow(edge1_win, 0, 0);
    string dep_win = "colorized depth";
    namedWindow(dep_win);
    moveWindow(dep_win, 960, 0);
    string color_win = "rgb";
    namedWindow(color_win);
    moveWindow(color_win, 500, 0);
#ifdef USE_EDGE2
    string edge2_win = "edge2";
    namedWindow(edge2_win);
    moveWindow(edge2_win, 660, 0);
#endif

    Mat color_image = imread(cfn);
    Mat depth_image = imread(dfn);
    Mat edge_image;

    imshow(dep_win, depth_image);

    load_bin_to_buffer(datfn, dep_buffer, dep_buffer_size);
    vector<Vec4i> p_lines;
    cout << "from depth_img ==>\n";

    SmallDots sm(color_image, depth_image, dep_buffer);
    sm.go();


    if (sett->find_edge) {
        // HERE: find edge
        find_edge(depth_image, dep_buffer, edge_image, p_lines, sm);

        size_t results_size = results.size();
        //printf("size of results: %d ===>", (int)results_size);

        // if (results_size) {
        //     dd2d(results.at(0), sm);
        // }

        // show answers
        for (size_t ii = 0; ii < std::min(results_size, (size_t)sett->max_show_answer); ii++) {
            //printf("size of results: %d\t", (int)results.size());
            Vec11i z = results.at(ii);
            print_answer_string(z, ii);
            Scalar clr = Scalar((ii?0x7f:0), ((ii&&ii%2)?0x7f:0), 0xff);
            //printf("draw_answer_line: edge_image\n");
            draw_answer_line(edge_image, z, clr);
            //printf("draw_answer_line: color_image\n");
            draw_answer_line(color_image, z, clr);
            //printf("draw_answer_line: depth_image\n");
            draw_answer_line(depth_image, z, clr);
        }
        imshow(edge1_win, edge_image);
    }

    // draw it after all other drawings
    cv::circle(depth_image, sm.small_pt, 5, Scalar(0,255,0), 2);

    imshow(color_win, color_image);
#ifdef USE_EDGE2
    cout << "from color_img:\n";
    find_edge(color_image, dep_buffer, edge_image, p_lines);
    imshow(edge2_win, edge_image);
#endif


    int key = waitKey(0);
    if (key == 0x1B) {
        cout << "user break" << endl;
    } else if (key == 's') {
        string fn = "test_from_image.jpg";
        cout << "save to " << fn << endl;
        cv::imwrite(fn, color_image);
    } else if (key == ' ') {

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
bool get_3d_angle(const rs2::depth_frame& depth, float& degree)
{
    degree = INVALID_DEGREE;
    if (!depth) {
        return false;
    }
    if (results.size() == 0) {
        return false;
    }
    //printf("enter %s...\n", __func__);
    cv::Point pt1;
    cv::Vec3f xyz1;
    pt1.x = results[0][0];
    pt1.y = results[0][1];

    cv::Point pt2;
    cv::Vec3f xyz2;
    pt2.x = results[0][2];
    pt2.y = results[0][3];


    bool ret1 = rsutil::query_uv2xyz(depth, pt1, xyz1);
    bool ret2 = rsutil::query_uv2xyz(depth, pt2, xyz2);

    if (ret1) {
        xyz1 *= 1000;
        //printf("%s: xyz1: %.0f,%.0f,%.0f  ", __func__, xyz1[0], xyz1[1], xyz1[2]);
    } else {
        printf("ret1 failed\n");
        return false;
    }
    if (ret2) {
        xyz2 *= 1000;
        //printf("xyz2: %.0f,%.0f,%.0f  ", xyz2[0], xyz2[1], xyz2[2]);
    } else {
        printf("ret2 failed\n");
        return false;
    }

    float dist3d = rsutil::dist_3d_xyz(xyz1, xyz2);
    printf("    dist3d: %.2f ", dist3d);

    float dx = xyz2[0] - xyz1[0];
    float dz = xyz2[2] - xyz1[2];

    bool ret = cvutil::get_angle_from_dx_dy(degree, dx, dz, false);
    return ret;
}


// main entry for realsense table edge finding
int test_realsense() try
{
    using namespace cv;

    ReadSetting* sett = ReadSetting::getInstance();
    bool isPaused = false;

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


    // Start streaming with default recommended configuration
    rs2::pipeline_profile profile = pipe.start(cfg);
    auto device = pipe.get_active_profile().get_device();
    depth_scale = rsutil::get_depth_scale(device);
    cout << "depth scale:" << depth_scale << endl;


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
        if (device.as<rs2::playback>() && isPaused) {
            int key = waitKey(1);
            if (key == ' ') {
                device.as<rs2::playback>().resume();
                isPaused = false;
            } else if (key == 0x1B) {
                printf("break from paused\n");
                break;
            }
            continue;
        }
        if (!pipe.poll_for_frames(&frameset)) {
            continue;
        }

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

        if (!depth || !color) {
            cout << "no depth or color frame\n";
            continue;
        }
        //cout << "frame got\n";
        rs2::frame colorized_depth = frameset.first(RS2_STREAM_DEPTH, RS2_FORMAT_RGB8);

        // if (sett->distance_limit > 0.0) {
        //     rsutil::remove_background(color, depth, depth_scale, sett->distance_limit);
        // }

        printf("[EPOCH] %ld =====>\n", cv::getTickCount());

        // Query frame size (width and height)
        //const int w = color.as<rs2::video_frame>().get_width();
        //const int h = color.as<rs2::video_frame>().get_height();
        int w = DEFAULT_WIDTH;
        int h = DEFAULT_HEIGHT;

        //cout << __LINE__ << "\n";
        // Create OpenCV matrix of size (w,h) from the colorized depth data
        Mat depth_image(Size(w, h), CV_8UC3, (void*)colorized_depth.get_data());
        Mat raw_color_image(Size(w, h), CV_8UC3, (void*)color.get_data());
        Mat orig_depth_image = depth_image.clone();
        Mat edge_image;

        Mat color_image;
        Mat orig_color_image;

        if (sett->distance_limit > 0.0) {
            rsutil::remove_background(color, depth_image, depth, depth_scale,
                sett->distance_limit);
        }

        if (sett->useBagFile()) {
            cvtColor(raw_color_image, color_image, COLOR_BGR2RGB);
        } else {
            color_image = raw_color_image.clone();
        }
        orig_color_image = color_image.clone();

        SmallDots sm(color_image, depth_image, depth.get_data());
        sm.go();

        if (sm.pass_check) {
            float degree3d = INVALID_DEGREE;
            if ( sm.get_3d_angle(depth, degree3d) ) {
                printf("    sm.get 3d angle: %.2f\n", degree3d);
            }
        }

        /// HERE try to find edge of table ... {
        if (sett->find_edge) {
            vector<Vec4i> p_lines;
            find_edge(depth_image, depth.get_data(), edge_image, p_lines, sm);
            size_t results_size = results.size();
            Vec11i last_z;

            // if (results_size) {
            //     dd2d(results.at(0), sm);
            // }


            for (size_t ii=0; ii<std::min(results_size, (size_t)sett->max_show_answer); ++ii) {
                Vec11i z = results.at(ii);
                print_answer_string(z, ii, sm.min_dist);
                Scalar clr = Scalar(((ii&&ii%2)?0x7f:0), (ii?0x7f:0), 0xff);
                //printf("draw_answer_line: edge_image\n");
                draw_answer_line(edge_image, z, clr);
                //printf("draw_answer_line: depth_image\n");
                draw_answer_line(depth_image, z, clr);

                #ifdef USE_GET_3DANGLE
                // get xyz vector angle...
                float degree3d = INVALID_DEGREE;
                if ( get_3d_angle(depth, degree3d) ) {
                    printf("3d deg:%.2f\n", degree3d);
                    if (fabs(degree3d) > MAX_DEGREE_LIMIT) {
                        printf("[trapped] degree too large\n");
                    } else {
                        if (ii==0) {
                            last_z = handle_lastz(z, sm);
                        }
                    }

                } else {
                    printf("invalid 3d degree\n");
                }
                #endif

                //printf("draw_answer_line: color_image\n");
                draw_answer_line(color_image, last_z, color_answer);
            }

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

        // draw it after all other drawings
        cv::circle(depth_image, sm.small_pt, 5, medlavendar, 2);

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
        } else if (key == 'c' || key == 0x0D) {
            save_procedure(orig_color_image, orig_depth_image, edge_image, depth.get_data());
        } else if (key == ' ') {
            if (device.as<rs2::playback>()) {
                if (isPaused) {
                    device.as<rs2::playback>().resume();
                    isPaused = false;
                } else {
                    device.as<rs2::playback>().pause();
                    isPaused = true;
                }
            }
        }
#endif  // USE_UIPRESENT
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
