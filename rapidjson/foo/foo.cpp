#include "pbox.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

#define DATAPATH    "src/ccbox/data/"
#define JSONFILE    "test.json"

void test()
{
    string json_file = pbox::get_home() + '/' + DATAPATH + JSONFILE;
    cout << "json_file: " << json_file << "\n";
}
