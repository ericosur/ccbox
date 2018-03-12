#ifndef __PBOX_H__
#define __PBOX_H__

#include <string>
#include <vector>

#ifdef USE_REALSENSE
#include <opencv2/opencv.hpp>   // Include OpenCV API
#endif

#include "MY_IPC.hpp"

namespace pbox {


std::string get_home();

bool is_file_exist(const char* fname);
bool is_file_exist(const std::string& fname);

void test();

void dump_jsonfile(const std::string& fn);
std::vector<std::string> get_vector_from_jsonfile(const std::string& fn,
                                                  std::vector<std::string> keys);

std::string get_string_from_jsonfile(const std::string& fn, const std::string& key);

double get_double_from_jsonfile(const std::string& fn,
                                std::vector<std::string> keys);

double get_double_from_jsonfile(const std::string& fn,
                                const std::string& key);

int get_int_from_jsonfile(const std::string& fn,
                          std::vector<std::string> keys);

int get_int_from_jsonfile(const std::string& fn,
                          const std::string& key,
                          int _default=0);

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

bool is_dog(int label_id);
bool is_person(int label_id);
std::string get_label_name(int label_id);
std::string output_detections(const std::string& ofn, const std::vector< std::vector<float> >& detections, int img_cols, int img_rows);

void init_realsense();
float get_dist_from_point(int x, int y);
bool get_color_mat(cv::Mat& image);
int get_color_mat_from_realsense(cv::Mat& image);
int test_realsense();
void draw_aim(cv::Mat& img, int x, int y, int w, int h);

#endif

}   // namespace pbox

#endif  // __PBOX_H__
