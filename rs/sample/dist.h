#ifndef __DIST_H__
#define __DIST_H__

#include <iostream>
// include the librealsense C++ header file
#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>

// include OpenCV header file
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

using pixel = std::pair<int, int>;

// Distance 3D is used to calculate real 3D distance between two pixels
float dist_3d(const rs2::depth_frame& frame, pixel u, pixel v);
// Distance 2D returns the distance in pixels between two pixels
float dist_2d(const pixel& a, const pixel& b);

void query_uv2xyz(const rs2::depth_frame& frame, const cv::Point& pt, cv::Vec3f& xyz, bool debug=false);

void test(const rs2::depth_frame& frame);

#endif
