#include "readset.h"

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
    if (load_json(JSON_FILE)) {

    }
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
    string tmpStr;
    int tmpVal;

    try {
        ifstream infile(JSON_FILE);
        infile >> json;

        tmpStr = json.at("host");
        host = tmpStr;
        cout << "host: " << host << endl;

        tmpVal = json.at("port");
        port = tmpVal;
        cout << "port: " << port << endl;

        tmpVal = json.at("max_recv_size");
        max_recv_size = tmpVal;
        cout << "max_recv_size: " << max_recv_size << endl;

    }
    catch (nlohmann::json::parse_error& e) {
        cout << "parse json error: " << e.what();
        return false;
    }
    return true;
}

#endif  // USE_JSON
