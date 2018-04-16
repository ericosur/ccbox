#ifndef __PUBLIC_MY_TOOLBOX_H__
#define __PUBLIC_MY_TOOLBOX_H__

#include <string>
#include <vector>

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>   // Include OpenCV API
#endif

namespace mytoolbox {

std::string get_home();

bool is_file_exist(const char* fname);
bool is_file_exist(const std::string& fname);
void dump(const char* buf, int size);

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


}   // namespace mytoolbox

#endif  // __PUBLIC_MY_TOOLBOX_H__
