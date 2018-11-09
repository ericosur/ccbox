#include "cvutil.h"
#include "readsetting.h"

//#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <time.h>

#define DEFAULT_CROP_WIDTH      240
#define DEFAULT_CROP_HEIGHT     240

using namespace std;

namespace cvutil {

int baseline = 0;

int get_timeepoch()
{
    time_t t = time(NULL);
    // will same as "date +%s"
    //printf("%d\n", (int)t);
    return (int)t;
}

std::string intToString(int number)
{
    std::stringstream ss;
    ss << number;
    return ss.str();
}

std::string get_save_image_fn(const string& prefix)
{
    std::string fn;
    fn = prefix + intToString(get_timeepoch()) + ".png";
    return fn;
}

std::string compose_image_fn(const string& prefix, int serial)
{
    std::string fn;
    fn = prefix + intToString(serial) + ".png";
    return fn;
}

std::string compose_depth_bin(const string& prefix, int serial)
{
    std::string fn;
    fn = prefix + intToString(serial) + ".bin";
    return fn;
}

#if 0
std::string save_mat_to_file(const cv::Mat& img)
{
    ReadSetting* s = ReadSetting::getInstance();
    std::string fn = s->get_save_image_fn();
    cv::imwrite(fn, img);
    return fn;
}
#endif

std::string save_mat_to_file_prefix(const cv::Mat& img, const std::string& prefix)
{
    std::string fn = get_save_image_fn(prefix);
    cv::imwrite(fn, img);
    return fn;
}


float get_iou(const cv::Rect& a, const cv::Rect& b)
{
    cv::Rect unionRect = a | b;
    cv::Rect intersectionRect = a & b;
    if (unionRect.area() == 0) {
        return 0.0;
    }
    float iou = (float)intersectionRect.area() / (float)unionRect.area();
    return iou;
}

void test_iou()
{
    cv::Rect a(80,120,50,50);
    cv::Rect b(100,100,50,50);
    float f = get_iou(a, b);
    printf("f: %.2f\n", f);
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

#if 1
void crop_image(const cv::Mat& orig, cv::Mat& new_img
    /*, int& rx, int& ry,
    bool do_imshow=false */)
{
    static bool isInited = false;
    int rx;
    int ry;
#ifdef DO_IMSHOW
    bool do_imshow=true;
#endif
    if (!isInited) {
        std::srand(std::time(nullptr)); // use current time as seed for random generator
        isInited = true;
    } else {
        // no need to srand again;
    }

    cout << "orig: " << orig.cols << " , " << orig.rows << endl;

    int base_x = 80;
    int base_y = 230;

    rx = base_x + std::rand() % 320;
    ry = base_y;

    int min_crop_width = 240;
    int min_crop_height = 90;

    crop_image(orig, new_img, rx, ry, min_crop_width, min_crop_height);
#ifdef DO_IMSHOW
    if (do_imshow) {
        imshow("cooo", new_img);
    }
#endif
}

#else
void crop_image(const cv::Mat& orig, cv::Mat& new_img)
{
    static bool isInited = false;

    if (!isInited) {
        std::srand(std::time(nullptr)); // use current time as seed for random generator
        isInited = true;
    } else {
        // no need to srand again;
    }

    int rx = std::rand() % (DEFAULT_WIDTH - DEFAULT_CROP_WIDTH);
    int base_y = DEFAULT_HEIGHT / 3;    // one-thrid of image
    int ry = base_y + std::rand() % (DEFAULT_HEIGHT - DEFAULT_CROP_HEIGHT - base_y);

    std::cout << "rx/ry:" << rx << ", " << ry << "\n";
    crop_image(orig, new_img, rx, ry, DEFAULT_CROP_WIDTH, DEFAULT_CROP_HEIGHT);
}
#endif

}   // namespace cvutil
