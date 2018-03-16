#ifndef __UTIL_SSD_SETTING_H__
#define __UTIL_SSD_SETTING_H__

#include <string>

using std::string;

class SsdSetting
{
public:
  bool fill_values(int argc, char** argv);
  bool read_values_from_json(const std::string& json_file);

public:
    string model_file;
    string weights_file;
    string file_type = "realsense";
    float confidence_threshold = 0.33;
    int label_id = 15;
    string label_name = "person";
    int direct_use_realsense = 0;
    int wait_msgq = 1;
    int wait_myipc = 0;
    string wait_myipc_tag = "photo_index";
    string dog_warning_tag = "dog_warning";
    string audience_vol_tag = "audience_vol";
    int do_imshow = 0;
    int show_fps = 1;
    int testraw = 1;
    string rawbin_dir = "/tmp";
    int test_crop = 0;
    int show_debug = 0;
    int max_crop_try = 3;
};


#endif  // __UTIL_SSD_SETTING_H__
