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
        video_id = pbox::get_int_from_jsonfile(json_file, "video_id", video_id);
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
        rawbin_dir = pbox::get_string_from_jsonfile(json_file, "rawbin_dir", rawbin_dir);
        test_crop = pbox::get_int_from_jsonfile(json_file, "test_crop");
        show_debug = pbox::get_int_from_jsonfile(json_file, "show_debug");
        max_crop_try = pbox::get_int_from_jsonfile(json_file, "max_crop_try", max_crop_try);
        vol_per_ratio = pbox::get_int_from_jsonfile(json_file, "vol_per_ratio", vol_per_ratio);

        dog_min_dist = pbox::get_int_from_jsonfile(json_file, "dog_min_dist", dog_min_dist);
        do_dog_alert = pbox::get_int_from_jsonfile(json_file, "do_dog_alert", do_dog_alert);
        do_man_alert = pbox::get_int_from_jsonfile(json_file, "do_man_alert", do_man_alert);

        if (file_type == "webcam") {
            printf("using webcam: auto turn off do_dog_alert and do_man_alert\n");
            do_dog_alert = 0;
            do_man_alert = 0;
        }

        low_vol = pbox::get_int_from_jsonfile(json_file, "low_vol", low_vol);
        mid_vol = pbox::get_int_from_jsonfile(json_file, "mid_vol", mid_vol);
        high_vol = pbox::get_int_from_jsonfile(json_file, "high_vol", high_vol);
        max_vol = pbox::get_int_from_jsonfile(json_file, "max_vol", max_vol);
        fill_volumes();

        pt1 = pbox::get_int_from_jsonfile(json_file, "pt1", pt1);
        pt2 = pbox::get_int_from_jsonfile(json_file, "pt2", pt2);
        pt3 = pbox::get_int_from_jsonfile(json_file, "pt3", pt3);
        threshold = pbox::get_int_from_jsonfile(json_file, "threshold", threshold);
        minus_offset = pbox::get_int_from_jsonfile(json_file, "minus_offset", minus_offset);
        plus_offset = pbox::get_int_from_jsonfile(json_file, "plus_offset", plus_offset);
        fill_ranges();

        host = pbox::get_string_from_jsonfile(json_file, "host", host);
        port = pbox::get_int_from_jsonfile(json_file, "port", port);
        max_recv_size = pbox::get_int_from_jsonfile(json_file, "max_recv_size", max_recv_size);

        return true;
    } else {
        recordlog("%s\n", "specified json not found...");
        return false;
    }
}

void SsdSetting::set_vol_epoch()
{
    last_vol_epoch = pbox::get_timeepoch();
}

void SsdSetting::set_dist_epoch()
{
    last_dist_epoch = pbox::get_timeepoch();
}

void SsdSetting::fill_volumes()
{
    volumes[VOL_ZERO] = 0;
    volumes[VOL_LOW] = low_vol;
    volumes[VOL_MID] = mid_vol;
    volumes[VOL_HIGH] = high_vol;
    volumes[VOL_MAX] = max_vol;
}

void SsdSetting::fill_ranges()
{
    range_inc[kRangeIncZero] = 0;
    range_inc[kRangeIncNear] = pt1 + plus_offset;
    range_inc[kRangeIncMiddle] = pt2 + plus_offset;
    range_inc[kRangeIncFar] = pt3 + plus_offset;
    range_inc[kRangeIncOut] = pt3 + plus_offset + 1;
    range_dec[kRangeDecZero] = 0;
    range_dec[kRangeDecNear] = pt1 - minus_offset;
    range_dec[kRangeDecMiddle] = pt2 - minus_offset;
    range_dec[kRangeDecFar] = pt3 - minus_offset;
    range_dec[kRangeDecOut] = pt3 + minus_offset;
}

