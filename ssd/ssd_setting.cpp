#include "ssd_setting.h"

#include <stdio.h>
#include <pbox/pbox.h>

bool SsdSetting::fill_values(int argc, char** argv)
{
    if (argc >= 7) {
        model_file = argv[1];
        weights_file = argv[2];
        file_type = argv[3];
        confidence_threshold = atof(argv[4]);
        label_id = atoi(argv[5]);
        label_name = argv[6];
        return true;
    } else {
        fprintf(stderr, "%s\n", "insufficient number of arguments...");
        return false;
    }
}

bool SsdSetting::read_values_from_json(const std::string& json_file)
{
    if ( pbox::is_file_exist(json_file) ) {
        model_file = pbox::get_string_from_jsonfile(json_file, "model_file");
        weights_file = pbox::get_string_from_jsonfile(json_file, "weights_file");
        file_type = pbox::get_string_from_jsonfile(json_file, "file_type");
        confidence_threshold = pbox::get_double_from_jsonfile(json_file, "confidence_threshold");
        label_id = pbox::get_int_from_jsonfile(json_file, "label_id");
        label_name = pbox::get_string_from_jsonfile(json_file, "label_name");
        direct_use_realsense = pbox::get_int_from_jsonfile(json_file, "direct_use_realsense");
        wait_msgq = pbox::get_int_from_jsonfile(json_file, "wait_msgq");
        wait_myipc = pbox::get_int_from_jsonfile(json_file, "wait_myipc");
        wait_myipc_tag = pbox::get_string_from_jsonfile(json_file, "wait_myipc_tag");
        dog_warning_tag = pbox::get_string_from_jsonfile(json_file, "dog_warning_tag");
        audience_vol_tag = pbox::get_string_from_jsonfile(json_file, "audience_vol_tag");
        do_imshow = pbox::get_int_from_jsonfile(json_file, "do_imshow");
        show_fps = pbox::get_int_from_jsonfile(json_file, "show_fps");
        testraw = pbox::get_int_from_jsonfile(json_file, "testraw");
        rawbin_dir = pbox::get_string_from_jsonfile(json_file, "rawbin_dir");
        test_crop = pbox::get_int_from_jsonfile(json_file, "test_crop");
        show_debug = pbox::get_int_from_jsonfile(json_file, "show_debug");
        max_crop_try = pbox::get_int_from_jsonfile(json_file, "max_crop_try");
        return true;
    } else {
        fprintf(stderr, "%s\n", "specified json not found...");
        return false;
    }
}
