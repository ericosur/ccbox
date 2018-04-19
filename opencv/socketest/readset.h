#ifndef __UTIL_READ_SET_H__
#define __UTIL_READ_SET_H__

#include <string>
#include <iostream>
#include <memory>
#include <cstdint>

#ifdef USE_JSON
#include <fstream>
#include <nlohmann/json.hpp>
#define JSON_FILE    "../setting.json"
#endif

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
    std::string host = "localhost";
    int port = 9801;
    int max_recv_size = 4096;

};


#endif  // __UTIL_READ_SET_H__
