#ifndef __COMMON_SETT_H__
#define __COMMON_SETT_H__

#include <string>
#include <vector>

bool is_file_exist(const std::string& fname);
bool check_files(const std::vector<std::string>& files);

bool load_json(const std::string& json_fn, std::vector<std::string>& files, int& threshold);
bool load_json(const std::string& json_fn, std::vector<std::string>& files);

#endif  // __COMMON_SETT_H__
