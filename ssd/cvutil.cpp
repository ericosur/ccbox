#include "cvutil.h"

#include <cstdlib>
#include <iostream>
#include <ctime>

#define DEFAULT_CROP_WIDTH      240
#define DEFAULT_CROP_HEIGHT     240

void draw_aim(cv::Mat& img, int x, int y, int w, int h)
{
    using namespace cv;

    const int radius = 30;
    const int tail = 5;
    Scalar white = Scalar(255, 255, 255);
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
    bool do_imshow=false)
{
    static bool isInited = false;

    if (!isInited) {
        std::srand(std::time(nullptr)); // use current time as seed for random generator
        isInited = true;
    } else {
        // no need to srand again;
    }

    rx = std::rand() % (DEFAULT_WIDTH - DEFAULT_CROP_WIDTH);
    int base_y = DEFAULT_HEIGHT / 3;    // one-thrid of image
    ry = base_y + std::rand() % (DEFAULT_HEIGHT - DEFAULT_CROP_HEIGHT - base_y);

    crop_image(orig, new_img, rx, ry, DEFAULT_CROP_WIDTH, DEFAULT_CROP_HEIGHT);
    // if (bShowDebug) {
    //     std::cout << "rx/ry:" << rx << ", " << ry
    //         << "//" << rx+DEFAULT_WIDTH << "," << ry+DEFAULT_HEIGHT << "\n";
    //     std::cout << "new_img:" << new_img.cols << "," << new_img.rows << "\n";
    // }

    if (do_imshow) {
        imshow("cooo", new_img);
    }

}

