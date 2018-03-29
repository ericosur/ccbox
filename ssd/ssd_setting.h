#ifndef __UTIL_SSD_SETTING_H__
#define __UTIL_SSD_SETTING_H__

#include <stdio.h>
#include <string>
#include "ssdutil.h"

using std::string;

class SsdSetting
{
public:
    static SsdSetting* getInstance();
    ~SsdSetting();

    bool fill_values(int argc, char** argv);
    bool read_values_from_json(const std::string& json_file);

    void recordlog(const char* format, ...);

    void set_vol_epoch();
    void set_dist_epoch();
    void set_last_dist(int dist) {
        last_dist = dist;
        set_dist_epoch();
    }
    void set_last_vol(int vol) {
        last_vol = vol;
        set_vol_epoch();
    }

protected:
    static SsdSetting* _instance;
    SsdSetting();

    void fill_volumes();
    void fill_ranges();

public:
    string errorlog = "/tmp/ssderror.log";

    string model_file;
    string weights_file;
#ifdef USE_REALSENSE
    // will default use realsense first
    string file_type = "realsense";
#else
    string file_type = "webcam";
#endif
    float confidence_threshold = 0.33;
    int label_id = 15;
    string label_name = "person";
    int video_id = 0;

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
    int vol_per_ratio = 7;

    int dog_min_dist = 100;
    int do_dog_alert = 1;
    int do_man_alert = 1;

    int low_vol = 25;
    int mid_vol = 50;
    int high_vol = 95;
    int max_vol = 100;
    int volumes[VOL_ENUMSIZE];
    int range_inc[kRangeIncEnumSize];
    int range_dec[kRangeDecEnumSize];

    int pt1 = 150;
    int pt2 = 250;
    int pt3 = 350;
    int threshold = 99;
    int minus_offset = 15;
    int plus_offset = 25;

    // not in json setting
    int last_dist_epoch = 0;
    int last_vol_epoch = 0;
    int last_dist = 0;
    int last_vol = 0;
    int pass_threshold = 0;
    int skipped = 0;
    float last_score = 0;

private:
    FILE* fptr = NULL;
};


#endif  // __UTIL_SSD_SETTING_H__
