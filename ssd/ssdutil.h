#ifndef __UILT_SSD_UTIL_H__
#define __UILT_SSD_UTIL_H__

#include <string>

enum VolumeLevelEnum {
    VOL_ZERO = 0,
    VOL_LOW,
    VOL_MID,
    VOL_HIGH,
    VOL_MAX,
    VOL_ENUMSIZE
};

enum RangeIncreased {
    kRangeIncZero = 0,
    kRangeIncNear,
    kRangeIncMiddle,
    kRangeIncFar,
    kRangeIncOut,
    kRangeIncEnumSize
};

enum RangeDereased {
    kRangeDecZero = 0,
    kRangeDecNear,
    kRangeDecMiddle,
    kRangeDecFar,
    kRangeDecOut,
    kRangeDecEnumSize
};

bool is_dog(int label_id);
bool is_person(int label_id);
std::string get_label_name(int label_id);

VolumeLevelEnum get_vol_while_decrease(int dist);
VolumeLevelEnum get_vol_while_increase(int dist);


#endif
