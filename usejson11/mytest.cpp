#include "mytest.h"
#include "json11.hpp"

#include <iostream>
#include <fstream>

using namespace json11;
using namespace std;

#define JSON_FILE   "../all.json"

#define PRINT_FUNC_NAME() \
    std::cout << __func__ << " =====>>>" << std::endl

bool read_file_into_string(const std::string& fn, std::string& str);
bool write_string_into_file(const std::string& fn, const std::string& str);

void test0()
{
    PRINT_FUNC_NAME();
    const string simple_test = R"({"k1":"v1", "k2":42, "k3":["a",123,true,false,null]})";

    string err;
    const auto json = Json::parse(simple_test, err);

    std::cout << "k1: " << json["k1"].string_value() << "\n";
    std::cout << "k3: " << json["k3"].dump() << "\n";

    for (auto &k : json["k3"].array_items()) {
        std::cout << "    - " << k.dump() << "\n";
    }

}

void test1()
{
    PRINT_FUNC_NAME();
    string whole_content;
    string err;

    read_file_into_string(JSON_FILE, whole_content);
    //cout << __func__ << "whole_content: " << whole_content << endl;

    const auto json = Json::parse(whole_content, err);
    if (err.length()) {
        cout << "err: " << err << endl;
        return;
    }


    //jq '.query.results.channel.item.condition' ../all.json
    cout << json["query"]["results"]["channel"]["item"]["condition"].dump() << endl;

#define MYCHECKNULLSTRING(x) \
    ((x) == std::string(""))

    // returns null if out-of-index access
    string o = json["query"]["results"]["channels"]["items"]["conditions"].dump();
    if (MYCHECKNULLSTRING(o)) {
        cout << o << endl;
    }
}

void test2()
{
    PRINT_FUNC_NAME();
    std::map<string, string> m1 { { "a", "apple" }, { "d", "dog" } };
    m1.insert(std::pair<string, string>("e", "egg"));
    m1.insert(std::pair<string, string>("t", "tiger"));
    m1.insert(std::pair<string, string>("z", "zebra"));

    m1["e"] = "elephant";

    Json jm1 = Json(m1);
    cout << jm1.dump() << endl;

    write_string_into_file("t.json", jm1.dump());


}

bool read_file_into_string(const std::string& fn, std::string& str)
{
    ifstream ifs(fn);
    std::string content((std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()    ));
    str = content;
    return true;
}


bool write_string_into_file(const std::string& fn, const std::string& str)
{
    std::ofstream out(fn);
    out << str;
    out.close();
    return true;
}
