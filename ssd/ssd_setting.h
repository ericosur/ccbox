#ifndef __UTIL_SSD_SETTING_H__
#define __UTIL_SSD_SETTING_H__

#include <stdio.h>
#include <string>

using std::string;

class SsdSetting
{
public:
    static SsdSetting* getInstance();
    ~SsdSetting();

    bool fill_values(int argc, char** argv);
    bool read_values_from_json(const std::string& json_file);

    void recordlog(const char* format, ...);

protected:
    static SsdSetting* _instance;
    SsdSetting();

public:
    string errorlog = "/tmp/ssderror.log";

    string model_file;
    string weights_file;
#ifdef USE_REALSENSE
    string file_type = "realsense";
#else
    string file_type = "webcam";
#endif
    float confidence_threshold = 0.33;
    int label_id = 15;
    string label_name = "person";
    int direct_use_realsense = 0;
    int wait_msgq = 0;
    int wait_myipc = 1;
    string wait_myipc_tag = "photo_index";
    string dog_warning_tag = "dog_warning";
    string audience_vol_tag = "audience_vol";
    string volume_adjust_tag = "volume_adjust";
    int do_imshow = 0;
    int show_fps = 1;
    int testraw = 1;
    string rawbin_dir = "/tmp";
    int test_crop = 0;
    int show_debug = 0;
    int max_crop_try = 3;

private:
    FILE* fptr = NULL;
};


#endif  // __UTIL_SSD_SETTING_H__
