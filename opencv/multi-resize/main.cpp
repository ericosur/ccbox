#include "cvutil.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <mytool/mytool.h>

#define SETTING_JSON    "../setting.json"

void read_file_list(const std::string& jsonfile, std::vector<std::string>& filelist)
{
    using namespace std;
    vector<string> keys;
    keys.push_back("test");
    filelist = mytoolbox::get_vector_from_jsonfile(jsonfile, keys);
}

int main()
{
    using namespace std;
    vector<string> filelist;
    read_file_list(SETTING_JSON, filelist);

    for (string fn: filelist) {
        if (mytoolbox::is_file_exist(fn)) {
            cout << "resize fn: " << fn << endl;
            resize_file(fn);
        } else {
            cout << "file not found: " << fn << endl;
        }
    }

    return 0;
}
