#ifndef __UTIL_READ_SETTING_H__
#define __UTIL_READ_SETTING_H__

#include <string>
#include <iostream>
#include <memory>

#ifdef USE_JSON
#include <fstream>
#include <nlohmann/json.hpp>
#define JSON_FILE    "../setting.json"
#endif

#include "cvutil.h"

#define MAX_SAVE_IMAGE_INDEX    9999

using namespace std;

class ReadSetting
{
public:
    static ReadSetting* getInstance();

    static bool is_file_exist(const std::string& fn);

#ifdef USE_JSON
    bool load_json(const std::string& json_file);
#endif

    std::string get_save_image_fn() {
        std::string img = string_format("capt%04d.png", save_image_index);
        std::string ret = output_dir + "/" + img;
        save_image_index ++;
        if (save_image_index >= MAX_SAVE_IMAGE_INDEX) {
            save_image_index = 0;
        }
        return ret;
    }

    void show(const std::string& str, bool true_false);
    void show(const std::string &str, int val);

protected:
    static ReadSetting* _instance;
    ReadSetting();
    template<typename ... Args>
    string string_format(const string& format, Args ... args){
        size_t size = 1 + snprintf(nullptr, 0, format.c_str(), args ...);
        unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args ...);
        return string(buf.get(), buf.get() + size);
    }

public:
    int video_id = 0;
    int default_width = DEFAULT_WIDTH;
    int default_height = DEFAULT_HEIGHT;
    bool use_realsense = true;
    bool use_edge_test = true;
    bool use_hsv_test = true;
    std::string output_dir = "out";

#ifdef USE_REALSENSE
    bool show_dist = true;
    bool show_fps = true;
    int color_scheme = 2;
    bool apply_sleep = false;
    bool apply_align = true;
    bool apply_dec = true;
    bool apply_disparity = true;
    bool apply_temporal = true;
    bool apply_spatial = true;
    bool apply_holefill = true;
#endif

private:
    int save_image_index = 0;
};


#endif  // __UTIL_READ_SETTING_H__
