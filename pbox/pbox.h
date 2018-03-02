#ifndef __PBOX_H__
#define __PBOX_H__

#include <string>
#include <vector>

#ifdef USE_REALSENSE
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
double get_double_from_jsonfile(const std::string& fn,
                              std::vector<std::string> keys);
int get_int_from_jsonfile(const std::string& fn,
                          std::vector<std::string> keys);

template<class T>
bool get_value_from_jsonfile(const std::string& json_file,
                        std::vector<std::string> keys,
                        T& value);

int get_timeepoch();

#ifdef USE_REALSENSE

void init_realsense();
float get_dist_from_point(int x, int y);
int get_color_mat_from_realsense(cv::Mat& image);
int test_realsense();

#endif

}   // namespace pbox

#endif  // __PBOX_H__
