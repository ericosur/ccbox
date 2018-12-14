#ifndef __UTIL_CV_UTIL_H__
#define __UTIL_CV_UTIL_H__

#include "cv_custom_type.h"

#include <cstdint>
#include <string>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>

#define DEFAULT_WIDTH    640
#define DEFAULT_HEIGHT   360

namespace cvutil {

const int fontface = cv::FONT_HERSHEY_SIMPLEX;
const double scale = 1;
const int thickness = 2;


void draw_aim(cv::Mat& img, int w, int h);
void draw_dist(cv::Mat& img, float dist);
void crop_image(const cv::Mat& orig, cv::Mat& new_img, int x, int y, int w, int h);
void crop_image(const cv::Mat& orig, cv::Mat& new_img);

int get_timeepoch();
std::string compose_image_fn(const std::string& prefix, int serial);
std::string compose_depth_bin(const std::string& prefix, int serial);
#if 0
std::string save_mat_to_file(const cv::Mat& img);
#endif
std::string save_mat_to_file_prefix(const cv::Mat& img, const std::string& prefix);
inline void save_mat_to_file(const cv::Mat& img, const std::string& fn)
{
    cv::imwrite(fn, img);
}

float get_iou(const cv::Rect& a, const cv::Rect& b);
void test_iou();

int get_points_between_two_points(const cv::Mat& img, const cv::Point& p1, const cv::Point& p2, std::vector<cv::Point>& points);

bool get_angle_from_dx_dy(double& degree, double dx, double dy, bool debug = false);

#ifdef USE_REALSENSE
bool check_point2(int x1, int y1, int z1, int x2, int y2, int z2, double& degree, bool debug=false);
bool check_point(int x1, int y1, int x2, int y2, double& degree, bool debug=false);
#endif
int get_avg_depth_from_points(const std::vector<int>& all_depth_results);
int get_median_depth_from_points(const std::vector<int>& all_depth_results);
bool show_cvfps(cv::Mat& cv_img, double elapsed_time);
void draw_crosshair(cv::Mat& img, const cv::Point& pt);

}   // namespace cvutil

#endif  // __UTIL_CV_UTIL_H__
