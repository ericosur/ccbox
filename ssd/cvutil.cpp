#include "cvutil.h"
#include "ssd_setting.h"

#include <cstdlib>
#include <iostream>
#include <ctime>

#define DEFAULT_CROP_WIDTH      240
#define DEFAULT_CROP_HEIGHT     240

const cv::Scalar white = cv::Scalar(255, 255, 255);

void draw_aim(cv::Mat& img, int x, int y, int w, int h)
{
    using namespace cv;

    const int radius = 30;
    const int tail = 5;
    int cx = x + w/2;
    int cy = y + h/2;

    circle(img, Point(cx, cy), radius, white, thickness);
    line(img, Point(cx-radius-tail, cy), Point(cx+radius+tail, cy), white, thickness);
    line(img, Point(cx, cy-radius-tail), Point(cx, cy+radius+tail), white, thickness);
}

void draw_aim(cv::Mat& img, int w, int h)
{
    using namespace cv;

    const int radius = 30;
    const int tail = 5;
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
    int baseline = 0;

    snprintf(buffer, buffer_size, "dist: %.3fm", dist);
    cv::Size text = cv::getTextSize(buffer, fontface, scale, thickness, &baseline);
    cv::rectangle(img, cv::Point(0, 0),cv::Point(text.width, text.height + baseline),
          CV_RGB(255, 255, 255), CV_FILLED);
    cv::putText(img, buffer, cv::Point(0, text.height + baseline / 2.),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

void crop_image(const cv::Mat& orig, cv::Mat& new_img, int x, int y, int w, int h)
{
    using namespace cv;

    cv::Rect roi(x, y, DEFAULT_CROP_WIDTH, DEFAULT_CROP_HEIGHT);
    new_img = orig(roi);


}

void crop_image(const cv::Mat& orig, cv::Mat& new_img, int& rx, int& ry,
    //int& min_crop_width, int& min_crop_height,
    int idx = 0,
    bool do_imshow=false)
{
    static bool isInited = false;
    SsdSetting* settings = SsdSetting::getInstance();

    if (!isInited) {
        std::srand(std::time(nullptr)); // use current time as seed for random generator
        isInited = true;
    } else {
        // no need to srand again;
    }

    const int base_x = 80;
    const int base_y = 230;
    int min_crop_width = 220;
    const int min_crop_height = 90;

    //rx = base_x + std::rand() % 320;
    rx = base_x + idx * 80;
    ry = base_y;

    if (settings->show_debug) {
        printf("%s: rx:%d,ry:%d\n", __func__, rx, ry);
    }

    // if (settings->show_debug) {
    //     std::cout << "rx/ry:" << rx << ", " << ry
    //         << "//" << rx+min_crop_width << "," << ry + min_crop_height << "\n";
    //     std::cout << "min_crop_width:" << min_crop_width << "," << min_crop_height << "\n";
    // }

    crop_image(orig, new_img, rx, ry, min_crop_width, min_crop_height);

    //if (do_imshow) {
        //imshow("cooo", new_img);
    //}
}

bool in_the_range_of(const int pt, const int target, const int threshold)
{
  int min = target - threshold;
  int max = target + threshold;
  if (min < 0) {
    min = 0;
    return false;
  }
  if (pt > min && pt < max) {
    return true;
  }
  return false;
}

