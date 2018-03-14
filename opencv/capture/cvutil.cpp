#include "cvutil.h"

namespace pbox {

int baseline = 0;

void draw_aim(cv::Mat& img, int w, int h)
{
    using namespace cv;

    const int radius = 30;
    const int tail = 5;
    Scalar white = Scalar(255, 255, 255);
    int cx = w/2;
    int cy = h/2;

    circle(img, Point(w/2, h/2), radius, white, thickness);
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



}   // namespace pbox
