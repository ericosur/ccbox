#ifndef __UTIL_CV_UTIL_H__
#define __UTIL_CV_UTIL_H__

#include <cstdint>
#include <opencv2/opencv.hpp>   // Include OpenCV API

#define DEFAULT_WIDTH    640
#define DEFAULT_HEIGHT   480

namespace pbox {

const int fontface = cv::FONT_HERSHEY_SIMPLEX;
const double scale = 1;
const int thickness = 2;


void draw_aim(cv::Mat& img, int w, int h);
void draw_dist(cv::Mat& img, float dist);
void crop_image(const cv::Mat& orig, cv::Mat& new_img, int x, int y, int w, int h);
void crop_image(const cv::Mat& orig, cv::Mat& new_img);

}   // namespace pbox

#endif  // __UTIL_CV_UTIL_H__
