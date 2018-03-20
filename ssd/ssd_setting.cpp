#include "ssd_setting.h"

#include <pbox/pbox.h>
#include <stdio.h>
#include <stdarg.h>

SsdSetting* SsdSetting::_instance = NULL;
SsdSetting* SsdSetting::getInstance()
{
    if (_instance == NULL) {
        _instance = new SsdSetting();
    }
    return _instance;
}

SsdSetting::SsdSetting()
{
    fptr = fopen(errorlog.c_str(), "at");
    if (fptr == NULL) {
        fprintf(stderr, "failed to output errorlog: %s\n", errorlog.c_str());
    }
}

SsdSetting::~SsdSetting()
{
    if (fptr != NULL) {
        fclose(fptr);
    }
}

void SsdSetting::recordlog(const char* format, ...)
{
    const int UTIL_DEFAULT_MYLOG_BUFFER_SIZE = 256;
    char buffer[UTIL_DEFAULT_MYLOG_BUFFER_SIZE];
    va_list aptr;
    int ret;

    va_start(aptr, format);
    ret = vsprintf(buffer, format, aptr);
    va_end(aptr);
    (void)ret;

    if (fptr != NULL) {
        fprintf(fptr, "[%d][%s]: %s\n", pbox::get_timeepoch(), "ssderror", buffer);
    }

    fflush(fptr);
}

/*
    WARNING: CLI arguments are not enough for full operation
 */
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
        recordlog("%s\n", "insufficient number of arguments...");
        return false;
    }
}

bool SsdSetting::read_values_from_json(const std::string& json_file)
{
    if ( pbox::is_file_exist(json_file) ) {
        model_file = pbox::get_string_from_jsonfile(json_file, "model_file", model_file);
        weights_file = pbox::get_string_from_jsonfile(json_file, "weights_file", weights_file);
        file_type = pbox::get_string_from_jsonfile(json_file, "file_type", file_type);
        confidence_threshold = pbox::get_double_from_jsonfile(json_file, "confidence_threshold", confidence_threshold);
        label_id = pbox::get_int_from_jsonfile(json_file, "label_id", label_id);
        label_name = pbox::get_string_from_jsonfile(json_file, "label_name", label_name);
        direct_use_realsense = pbox::get_int_from_jsonfile(json_file, "direct_use_realsense", direct_use_realsense);
        wait_msgq = pbox::get_int_from_jsonfile(json_file, "wait_msgq", wait_msgq);
        wait_myipc = pbox::get_int_from_jsonfile(json_file, "wait_myipc", wait_myipc);
        wait_myipc_tag = pbox::get_string_from_jsonfile(json_file, "wait_myipc_tag", wait_myipc_tag);
        dog_warning_tag = pbox::get_string_from_jsonfile(json_file, "dog_warning_tag", dog_warning_tag);
        audience_vol_tag = pbox::get_string_from_jsonfile(json_file, "audience_vol_tag", audience_vol_tag);
        volume_adjust_tag = pbox::get_string_from_jsonfile(json_file, "volume_adjust_tag", volume_adjust_tag);
        do_imshow = pbox::get_int_from_jsonfile(json_file, "do_imshow");
        show_fps = pbox::get_int_from_jsonfile(json_file, "show_fps");
        testraw = pbox::get_int_from_jsonfile(json_file, "testraw");
        rawbin_dir = pbox::get_string_from_jsonfile(json_file, "rawbin_dir");
        test_crop = pbox::get_int_from_jsonfile(json_file, "test_crop");
        show_debug = pbox::get_int_from_jsonfile(json_file, "show_debug");
        max_crop_try = pbox::get_int_from_jsonfile(json_file, "max_crop_try");
        return true;
    } else {
        recordlog("%s\n", "specified json not found...");
        return false;
    }
}
