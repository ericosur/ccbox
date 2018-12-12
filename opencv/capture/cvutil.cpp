#include "cvutil.h"
#include "readsetting.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <time.h>
#include <math.h>

#define DEFAULT_CROP_WIDTH      240
#define DEFAULT_CROP_HEIGHT     240

using namespace std;

#if 0
#define show_line() \
    printf("@line %04d\n", __LINE__);
#else
#define show_line()
#endif

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

    //cout << "orig: " << orig.cols << " , " << orig.rows << endl;

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

/// [in] img
/// [in] p1, p2
/// [out] points
int get_points_between_two_points(const cv::Mat& img, const cv::Point& p1, const cv::Point& p2, std::vector<cv::Point>& points)
{
    using namespace cv;
    LineIterator it(img, p1, p2);

    //cout << "from " << p1 << " to " << p2 << "\t";
    //cout << "it.count: " << it.count << endl;
    for (int i = 0; i < it.count; i ++) {
        // px is the color value of pixel
        //Vec3b px = Vec3b(*it);
        //cout << "pos: " << it.pos() << "\t";
        //cout << "bgr: " << px << endl;
        points.push_back(it.pos());
        it++;
    }
    return it.count;
}

void test_get_points_between_two_points()
{
    using namespace cv;
    Point p1 = Point(50, 5);
    Point p2 = Point(128, 153);
    Mat img = imread("../small.png");
    std::vector<cv::Point> points;

    int ret = get_points_between_two_points(img, p1, p2, points);
    if (ret) {
        for (size_t i=0; i<points.size(); i++) {
            cout << points.at(i) << endl;
        }
    }

}

#ifdef USE_REALSENSE
bool check_point2(int x1, int y1, int z1, int x2, int y2, int z2, double& degree, bool debug)
{
    if (z1 == 0 || z2 == 0) {
        if (debug) {
            cout << "0 ";
        }
        // useless depth
    } else {
        if (abs(z1 - z2) > 500) {
            // not good
            if (debug) {
                cout << "> ";
            }
            return false;
        }
    }
    return check_point(x1, y1, x2, y2, degree);
}

bool check_point(int x1, int y1, int x2, int y2, double& degree, bool debug)
{
    ReadSetting* sett = ReadSetting::getInstance();
    const int margin = 80;
    const float ang_too_slope = sett->max_degree;
    // area #1
    if (y1<margin && y2<margin)
        return false;
    if (y1>DEFAULT_HEIGHT-margin && y2>DEFAULT_HEIGHT-margin)
        return false;
    if (x1<margin && x2<margin)
        return false;
    if (x1>DEFAULT_WIDTH-margin && x2>DEFAULT_WIDTH-margin)
        return false;

    if (x1 == x2) {
        return false;
    }

    double dy = y2 - y1;
    double dx = x2 - x1;
    if (fabs(dx) < 10.0 || fabs(dy) < 1.0) {
        if (debug) {
            cout << "x ";
        }
        return false;
    }

    if (debug) {
        if (dy > 0 && dx > 0) {
            cout << "/ ";
        } else {
            cout << "\\ ";
        }
        if (dx == 0.0) {
            cout << "div ";
        }
    }

    double slope = dy / dx;
    if (debug) {
        cout << "dx " << dx << " dy " << dy << " slope: " << slope << endl;
    }

    const double pos_slope = 11.43;
    const double neg_slope = -pos_slope;
    const double pos_max_degree = 85.0;
    const double neg_max_degree = -pos_max_degree;
    if (slope > pos_slope) {
        show_line();
        degree = pos_max_degree;
    } else if (slope < neg_slope) {
        show_line();
        degree = neg_max_degree;
    } else {
        double _a = atan(slope);
        show_line();
        (void)_a;
        //cout << "_a: " << _a << endl;
        degree = _a * 180.0 / M_PI;
    }
    //cout << "dx " << dx << " dy " << dy << " slope: " << slope << " theta: " << degree << endl;
    if (debug) {
        cout << " theta: " << degree << endl;
    }
    if (degree > ang_too_slope) {
        return false;
    }

    return true;
}
#endif

/// [in] depth_data: raw data buffer of depth
/// [in] vector to store points
int get_avg_depth_from_points(const std::vector<int>& all_depth_results)
{
    int sum = 0;
    for (size_t ii=0; ii<all_depth_results.size(); ++ii) {
        sum += all_depth_results.at(ii);
    }

    return sum / all_depth_results.size();
}


int get_median_depth_from_points(const std::vector<int>& all_depth_results)
{
    int ans = 0;
    std::vector<int> v = all_depth_results;
    std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
    auto id1 = v.size() / 2;
    if ( all_depth_results.size() % 2 ) {   // size is odd
        ans = v[id1];
    } else {    // size is even
        if (v.size() / 2 >= 1) {
            auto id0 = v.size() / 2 - 1;
            ans = (v[id0] + v[id1]) / 2;
        }
    }

    return ans;
}

bool show_cvfps(cv::Mat& cv_img, double elapsed_time)
{
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.75;
    int baseline = 0;
    int thickness = 2;
    const int BUFFER_SIZE = 80;
    char buffer[BUFFER_SIZE];
    double fps = 1.0 / elapsed_time;

    snprintf(buffer, BUFFER_SIZE, "elapsed: %.3f fps(%.1f)", elapsed_time, fps);
    cv::Size text = cv::getTextSize(buffer, fontface, scale, thickness, &baseline);
    cv::rectangle(cv_img, cv::Point(0, 0),cv::Point(text.width, text.height + baseline),
          CV_RGB(255, 255, 255), CV_FILLED);
    cv::putText(cv_img, buffer, cv::Point(0, text.height + baseline / 2.),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);

    //cv::imshow(rgb_window, cv_img);

    return true;
}

void draw_crosshair(cv::Mat& img, const cv::Point& pt)
{
    using namespace cv;
    const int slen = 40;
    const int radius = 10;
    Scalar color = Scalar(0, 0, 0xff);
    circle(img, pt, radius, color);
    Point p1 = Point(pt.x - slen, pt.y );
    Point p2 = Point(pt.x + slen, pt.y );
    line(img, p1, p2, color, 1, LINE_AA);
    Point p3 = Point(pt.x, pt.y - slen);
    Point p4 = Point(pt.x, pt.y + slen);
    line(img, p3, p4, color, 1, LINE_AA);
}


}   // namespace cvutil
