#ifndef __UTIL_CV_UTIL_H__
#define __UTIL_CV_UTIL_H__

#include <cstdint>
#include <vector>
#include <opencv2/opencv.hpp>   // Include OpenCV API

#define DEFAULT_WIDTH             640
#define DEFAULT_HEIGHT            480
#define MAX_PERSON_IMAGE_COUNT    100

typedef enum tagReidName {
    kNone = -1,
    kBin = 1000,
    kAllen,
    kRasmus,
    kJoe,
    kPerson,
    kMaxNumber
} ReidName ;

const int fontface = cv::FONT_HERSHEY_SIMPLEX;
const double scale = 1;
const int thickness = 2;

#ifdef USE_SOCKET
int send_crop_image_to_server(const cv::Mat& orig_img, const cv::Rect& rr);
void setup_connect();
ReidName check_recv();
void send_string_to_server(const std::string& msg);
void transfer_image_to_server(const std::string& file);
#endif  // USE_SOCKET

int save_crop_image(const cv::Mat& orig_img, const cv::Rect& rr);
ReidName check_result();

void draw_aim(cv::Mat& img, int x, int y, int w, int h);
void draw_aim(cv::Mat& img, int w, int h);
void draw_dist(cv::Mat& img, float dist);
void crop_image(const cv::Mat& orig, cv::Mat& new_img,
                int x, int y, int w, int h);
void crop_image_rect(const cv::Mat& orig, cv::Mat& new_img, cv::Rect rr);
void crop_image(const cv::Mat& orig, cv::Mat& new_img,
                int& rx, int& ry,
                //int& min_crop_width, int& min_crop_height,
                int idx,
                bool do_imshow);

bool in_the_range_of(const int pt, const int target, const int threshold);

// get iou from two cv::rect
float get_iou(const cv::Rect& a, const cv::Rect& b);

class PersonRect;
int find_min_area(const std::vector<PersonRect> &v);
int find_max_area(const std::vector<PersonRect> &v);
int find_closest(const std::vector<PersonRect> &v);

class PersonRect
{
public:
  PersonRect() {};


  PersonRect(const PersonRect& src) {
    _rect = src._rect;
    score = src.score;
    dist = src.dist;
  }

  PersonRect(int xx, int yy, int ww, int hh, float ff, int dd) {
    _rect = cv::Rect(xx, yy, ww, hh);
    //printf("%d: area: %d\n", __LINE__, _rect.area());
    score = ff;
    dist = dd;
  }

  const cv::Rect get_rect() const {
    return _rect;
  }

  const int get_area() const {
    return (int)_rect.area();
  }

  const float get_score() const {
    return score;
  }

  const int get_dist() const {
    return dist;
  }

  static int get_count() {
    return count;
  }

  static int inc_count() {
    if (count + 1 > MAX_PERSON_IMAGE_COUNT) {
      count = 0;
    } else {
      count ++;
    }
    return count;
  }

private:
  cv::Rect _rect;
  float score = 0.0;
  int dist = 0;
  static int count;
};


#endif  // __UTIL_CV_UTIL_H__
