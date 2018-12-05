#ifndef __RS_UTIL_H__
#define __RS_UTIL_H__

#ifdef USE_REALSENSE

#include <iostream>
// include the librealsense C++ header file
#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>

// include OpenCV header file
#include <opencv2/opencv.hpp>

namespace rsutil {

//using namespace std;
//using namespace cv;

using pixel = std::pair<int, int>;

// Distance 3D is used to calculate real 3D distance between two pixels
float dist_3d(const rs2::depth_frame& frame, pixel u, pixel v);
// Distance 2D returns the distance in pixels between two pixels
float dist_2d(const pixel& a, const pixel& b);

void query_uv2xyz(const rs2::depth_frame& frame, const cv::Point& pt, cv::Vec3f& xyz, bool debug=false);

bool get_vec_deg(cv::Mat& v, double& degree, bool debug=false);

bool show_rsinfo();

float get_depth_scale(rs2::device dev);

void remove_background(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist);

}   // namespace rsutil

#endif  // USE_REALSENSE
#endif  // __RS_UTIL_H__
