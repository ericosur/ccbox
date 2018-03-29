#ifndef __PBOX_H__
#define __PBOX_H__

#include <string>
#include <vector>

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>   // Include OpenCV API
#endif

namespace pbox {

std::string get_home();

bool is_file_exist(const char* fname);
bool is_file_exist(const std::string& fname);

void test();

void dump_jsonfile(const std::string& fn);
std::vector<std::string> get_vector_from_jsonfile(const std::string& fn,
                                                  std::vector<std::string> keys);

std::string get_string_from_jsonfile(const std::string& fn,
                                     const std::string& key,
                                     const std::string& default_value = "");

double get_double_from_jsonfile(const std::string& fn,
                                std::vector<std::string> keys);

double get_double_from_jsonfile(const std::string& fn,
                                const std::string& key,
                                const double default_value = 0.0);

int get_int_from_jsonfile(const std::string& fn,
                          std::vector<std::string> keys);

int get_int_from_jsonfile(const std::string& fn,
                          const std::string& key,
                          const int default_value = 0);

template<class T>
bool get_value_from_jsonfile(const std::string& json_file,
                             std::vector<std::string> keys,
                             T& value);
template<class T>
bool get_value_from_jsonfile(const std::string& json_file,
                             const std::string& key, T& value);

int get_timeepoch();
std::string get_timestring();
std::string get_version_string();

void mylog(const char* name, const char* format, ...);

void output_status(const std::string& ofn, const std::vector<std::string>& v);



#ifdef USE_REALSENSE

std::string output_detections(const std::string& ofn, const std::vector< std::vector<float> >& detections, int img_cols, int img_rows);

void init_realsense();
int get_dist_from_point(int x, int y);
int get_rs_dpeth_pt2(int x, int y);
bool get_color_mat(cv::Mat& image);
int get_color_mat_from_realsense(cv::Mat& image);
int test_realsense();

#endif

#ifdef USE_OPENCV
void draw_aim(cv::Mat& img, int x, int y, int w, int h);
#endif

}   // namespace pbox

#endif  // __PBOX_H__
