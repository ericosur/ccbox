#include "ssdutil.h"
#include "ssd_setting.h"

bool is_dog(int label_id)
{
    if (label_id == 8 || label_id == 12) {
        return true;
    } else {
        return false;
    }
}

bool is_person(int label_id)
{
    // person
    if (label_id == 15) {
        return true;
    } else {
        return false;
    }
}

std::string get_label_name(int label_id)
{
    std::string result;
    switch (label_id) {
    case 8: // cat
        result = "dog";
        break;
    case 12:  // dog
        result = "dog";
        break;
    case 15:  // person
        result = "man";
        break;
    default:
        result = "null";
        break;
    }
    return result;
}

// new dist is less than old one
VolumeLevelEnum get_vol_while_decrease(int dist)
{
    VolumeLevelEnum result = VOL_ZERO;
    SsdSetting* s = SsdSetting::getInstance();
    if (dist > s->range_dec[kRangeDecZero] && dist <= s->range_dec[kRangeDecNear]) {
        result = VOL_LOW;
    } else if ( dist > s->range_dec[kRangeDecNear] && dist <= s->range_dec[kRangeDecMiddle] ) {
        result = VOL_MID;
    } else if ( dist > s->range_dec[kRangeDecMiddle] && dist <= s->range_dec[kRangeDecFar] ) {
        result = VOL_HIGH;
    } else if ( dist > s->range_dec[kRangeDecFar] ) {
        result = VOL_MAX;
    }

    return result;
/*
    VOL_ZERO = 0,
    VOL_LOW,
    VOL_MID,
    VOL_HIGH,
    VOL_MAX,
*/
}

VolumeLevelEnum get_vol_while_increase(int dist)
{
    VolumeLevelEnum result = VOL_ZERO;
    SsdSetting* s = SsdSetting::getInstance();
    if (dist > s->range_inc[kRangeIncZero] && dist <= s->range_inc[kRangeIncNear]) {
        result = VOL_LOW;
    } else if ( dist > s->range_inc[kRangeIncNear] && dist <= s->range_inc[kRangeIncMiddle] ) {
        result = VOL_MID;
    } else if ( dist > s->range_inc[kRangeIncMiddle] && dist <= s->range_inc[kRangeIncFar] ) {
        result = VOL_HIGH;
    } else if ( dist > s->range_dec[kRangeIncFar] ) {
        result = VOL_MAX;
    }

    return result;
}
