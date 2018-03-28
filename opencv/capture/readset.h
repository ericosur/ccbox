#ifndef __UTIL_READ_SET_H__
#define __UTIL_READ_SET_H__

#include <string>

#ifdef USE_JSON
#include <fstream>
#include <nlohmann/json.hpp>
#define JSON_FILE    "../setting.json"
#endif


class ReadSetting
{

public:
    static ReadSetting* getInstance();

    static bool is_file_exist(const std::string& fn);

#ifdef USE_JSON
    bool load_json(const std::string& json_file);
#endif

protected:
    static ReadSetting* _instance;
    ReadSetting();

public:
    int video_id = 0;
    bool use_realsense = true;
};




#endif  // __UTIL_READ_SET_H__
