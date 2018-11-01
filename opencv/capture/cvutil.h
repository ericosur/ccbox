#ifndef __UTIL_CV_UTIL_H__
#define __UTIL_CV_UTIL_H__

#include <cstdint>
#include <string>
#include <opencv2/opencv.hpp>   // Include OpenCV API

#define DEFAULT_WIDTH    640
#define DEFAULT_HEIGHT   480

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
std::string save_mat_to_file(const cv::Mat& img);
std::string save_mat_to_file_prefix(const cv::Mat& img, const std::string& prefix);
inline void save_mat_to_file(const cv::Mat& img, const std::string& fn)
{
    cv::imwrite(fn, img);
}

float get_iou(const cv::Rect& a, const cv::Rect& b);
void test_iou();

}   // namespace cvutil

#endif  // __UTIL_CV_UTIL_H__
