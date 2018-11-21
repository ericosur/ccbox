#include "sett.h"

#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdlib.h>
#include <unistd.h>


bool is_file_exist(const std::string& fname)
{
    if ( access(fname.c_str(), F_OK) != -1 ) {
        return true;
    }
    return false;
}

bool check_files(const std::vector<std::string>& files)
{
    for (size_t i = 0; i < files.size(); ++i) {
        auto fn = files.at(i);
        if (!is_file_exist(fn)) {
            std::cout << "file: " << fn << " not found!\n";
            return false;
        }
    }
    return true;
}

bool load_json(const std::string& json_fn, std::vector<std::string>& files, int& threshold)
{
    using namespace std;
    using namespace nlohmann;

    cout << "load settings from: " << json_fn << "\n";
    json j;
    json jarray = json::array();

    try {
        ifstream infile(json_fn);
        infile >> j;
        jarray = j.at("files");
        threshold = j.at("threshold");
        //cout << setw(4) << j << endl;
    }
    catch (json::parse_error& e) {
        cout << "parse json error: " << e.what();
        return false;
    }

    // maybe there is a more efficient way to load json array into std::vector
    for (json::iterator it = jarray.begin(); it != jarray.end(); ++it) {
        //cout << *it << endl;
        files.push_back(it->get<string>());
    }

    return true;
}

bool load_json(const std::string& json_fn, std::vector<std::string>& files)
{
    using namespace std;
    using namespace nlohmann;

    cout << "load settings from: " << json_fn << "\n";
    json j;
    json jarray = json::array();

    try {
        ifstream infile(json_fn);
        infile >> j;
        jarray = j.at("files");
        //cout << setw(4) << j << endl;
    }
    catch (json::parse_error& e) {
        cout << "parse json error: " << e.what();
        return false;
    }

    // maybe there is a more efficient way to load json array into std::vector
    for (json::iterator it = jarray.begin(); it != jarray.end(); ++it) {
        //cout << *it << endl;
        files.push_back(it->get<string>());
    }

    return true;
}
