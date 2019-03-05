#include "readsetting.h"
#include <unistd.h>


bool ReadSetting::is_file_exist(const std::string& fn)
{
    if (access(fn.c_str(), F_OK) == 0) {
        return true;
    } else {
        return false;
    }
}

#ifdef USE_JSON

#include <iostream>
#include <fstream>

ReadSetting* ReadSetting::_instance = NULL;
ReadSetting* ReadSetting::getInstance()
{
    if (_instance == NULL) {
        _instance = new ReadSetting();
    }
    return _instance;
}

ReadSetting::ReadSetting()
{
}

bool ReadSetting::read_setting()
{
    bool ret = load_json(JSON_FILE);
    if (ret) {
        isOpened = true;
    } else {
        cout << "[ERROR] load json error\n";
        isOpened = false;
    }
    return ret;
}

void ReadSetting::show(const std::string& str, bool true_false)
{
    cout << str << ": " << (true_false ? "true" : "false") << endl;
}

void ReadSetting::show(const std::string& str, int val)
{
    cout << str << " = " << val << endl;
}

void ReadSetting::show(const std::string& str, float val)
{
    cout << str << " = " << val << endl;
}

bool ReadSetting::load_json(const std::string& json_file)
{
    using namespace std;

    cout << "load settings from: " << json_file << "\n";
    if (!is_file_exist(JSON_FILE)) {
        cout << "json not found..." << endl;
        return false;
    }
    nlohmann::json json;
    try {
        ifstream infile(JSON_FILE);
        infile >> json;

        int vid = json.at("video_id");
        cout << "video_id: " << vid << endl;
        video_id = vid;

        // default_width = json.at("default_width");
        // show("default_width", default_width);
        // default_height = json.at("default_height");
        // show("default_height", default_height);

        use_realsense = json.at("use_realsense");

        bool tmp = json.at("use_edge_test");
        use_edge_test = tmp;
        tmp = json.at("use_hsv_test");
        use_hsv_test = tmp;

        std::string _dir = json.at("output_dir");
        output_dir = _dir;

        bool _debug = json.at("debug");
        debug = _debug;

#ifdef USE_REALSENSE
        nlohmann::json jsub = json.at("realsense");

        find_edge = jsub.at("find_edge");
        blur_radius = jsub.at("blur_radius");
        canny_threshold_min = jsub.at("canny_threshold_min");
        canny_threshold_max = jsub.at("canny_threshold_max");
        hough_threshold = jsub.at("hough_threshold");
        hough_minlength = jsub.at("hough_minlength");
        hough_maxlinegap = jsub.at("hough_maxlinegap");
        show_dist = jsub.at("show_dist");
        //show("show_dist", show_dist);
        show_fps = jsub.at("show_fps");
        //show("show_fps", show_fps);
        color_scheme = jsub.at("color_scheme");
        //show("color_scheme", color_scheme);
        distance_limit = jsub.at("distance_limit");
        show("distance_limit", distance_limit);

        apply_align = jsub.at("apply_align");
        show("apply_align", apply_align);
        apply_dec = jsub.at("apply_dec");
        show("apply_dec", apply_dec);
        apply_disparity = jsub.at("apply_disparity");
        show("apply_disparity", apply_disparity);
        apply_spatial = jsub.at("apply_spatial");
        show("apply_spatial", apply_spatial);
        apply_temporal = jsub.at("apply_temporal");
        show("apply_temporal", apply_temporal);
        apply_holefill = jsub.at("apply_holefill");
        show("apply_holefill", apply_holefill);

        max_degree = jsub.at("max_degree");
        show("max_degree", max_degree);

        ignore_margin = jsub.at("ignore_margin");
        show("ignore_margin", ignore_margin);

        max_show_answer = jsub.at("max_show_answer");
        show("max_show_answer", max_show_answer);
#endif
    }
    catch (nlohmann::json::parse_error& e) {
        cout << "parse json error: " << e.what();
        return false;
    }
    return true;
}

#endif  // USE_JSON
