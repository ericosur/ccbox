#include "foo.h"

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <json.hpp>

using namespace std;
using namespace nlohmann;

bool is_file_exist(const char* fname)
{
    if ( access(fname, F_OK) != -1 ) {
        return true;
    } else {
        return false;
    }
}

bool get_json_from_file(const string& json_file, json& j)
{
    if ( !is_file_exist(json_file.c_str()) ) {
        return false;
    }

    //cout << "read " << json_file << endl;
    try {
        ifstream inf(json_file);
        inf >> j;
        return true;
    } catch (json::parse_error& e) {
        cout << "[get_json_from_file] parse error:" << e.what() << endl;
    }

    return false;
}

void dump_jsonfile(const std::string& fn)
{
    json r;
    if ( get_json_from_file(fn, r) ) {
        cout << r.dump(4);
    } else {
        cout << "dump_jsonfile: failed to dump" << endl;
    }
}


vector<string> get_vector_from_json(json& j, vector<string> keys)
{
    std::vector<string> empty;

     try {
        json kk = j;
        for (string i : keys) {
            kk = kk.at(i);
        }
        return kk.get<std::vector<string>>();
    } catch (json::type_error& e) {
        cout << "[get_vector_from_json] type error:" << e.what() << endl;
    } catch (json::parse_error& e) {
        cout << "[get_vector_from_json] parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
        cout << "[get_vector_from_json] out of range:" << e.what() << endl;
    }

    return empty;
}

vector<string> get_vector_from_jsonfile(const string& fn, vector<string> keys)
{
    json j;
    std::vector<string> empty;
    if (get_json_from_file(fn, j)) {
        return get_vector_from_json(j, keys);
    } else {
        return empty;
    }
}


void to_json(json& j, const Person& p)
{
    j = json{
        {"name", p.getName()},
        {"address", p.getAddress()},
        {"age", p.getAge()}
    };
}

void from_json(const json& j, Person& p)
{
    p.setName( j.at("name").get<std::string>() );
    p.setAddress( j.at("address").get<std::string>() );
    p.setAge( j.at("age").get<int>() );
}

void test_foo()
{
    //Person p = {"Alice Smith", "Taipei City", 30};
    Person p;
    p.setName("Alice Smith");
    p.setAddress("Taipei City");
    p.setAge(30);

    // convert **Person** to **json**
    json j = p;
    cout << j << endl;

    // convert **json** to **p2**
    Person p2 = j;

    assert(p == p2);
}

void test_bar()
{
    // create an object from an object_t value
    json::object_t object_value = { {"one", 1}, {"two", 2} };
    json j_object_t(object_value);
    cout << "t_object_t: " << j_object_t.dump(4) << endl;

    // create an object from std::map
    std::map<std::string, int> c_map
    {
        {"apple", 100}, {"whiskey", 300}, {"tiger", 250}
    };
    json j_map(c_map);
    cout << "c_map: " << j_map.dump(4) << endl;
    // string str = j_map.dump();
    // cout << "str: " << str << endl;
}

void test_weather()
{
    ifstream inf("../query.txt");
    json jj;
    inf >> jj;
    //cout << jj.dump(4) << endl;

    string created = jj["query"]["created"].get<string>();
    cout << "created: " << created << endl;

    map<string, string> condition = jj["query"]["results"]["channel"]["item"]["condition"].get<map<string, string>>();

    condition["hello"] = "world";

    for (map<string,string>::iterator it=condition.begin(); it!=condition.end(); ++it)
        cout << it->first << " => " << it->second << '\n';

    //cout << "temp:" << condition["temp"] << endl;

}

void test_obj()
{
    string json_file = "notexist.json";
    cout << "read " << json_file << endl;
    try {
        ifstream inf(json_file);
        json j;
        inf >> j;

        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            for (auto& e: j[it.key()]) {
                string key = it.key();
                cout << key << ": " << e << endl;
            }
        }
    } catch (json::parse_error& e) {
        cout << "parse error:" << e.what() << endl;
    }
}

string get_from_id(const string& msgid)
{
    string json_file = "../strdef.json";
    cout << "read " << json_file << endl;
    try {
        ifstream inf(json_file);
        json jj;
        inf >> jj;

        string id = jj.at("General").at(msgid);
        string foo = jj.at("ar_AE").at(id);

        return foo;
    } catch (json::out_of_range& e) {
       std::cout << "out of range:" << e.what() << '\n';
    }

    return "";
}

void test_addon()
{
    string json_file = "../apps-add-ons.json";
    cout << "read " << json_file << endl;
    try {
        ifstream inf(json_file);
        json j;
        inf >> j;

        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            json item = (*it);
            cout << item.at("name") << endl
                << item.at("icon") << endl;
        }
    } catch (json::parse_error& e) {
        cout << "parse error:" << e.what() << endl;
    }
}

void test()
{
    // test_foo();
    // cout << endl;
     test_bar();
     cout << endl;
    //test_weather();
    //test_addon();


    char text[] = R"(
    {
        "Image": {
            "Width":  800,
            "Height": 600,
            "Title":  "View from 15th Floor",
            "Thumbnail": {
                "Url":    "http://www.example.com/image/481989943",
                "Height": 125,
                "Width":  100
            },
            "Animated" : false,
            "IDs": [
                "apple",
                "ball",
                "cat"
            ]
        }
    }
    )";

    json j = json::parse(text);
    vector<string> keys;
    keys.push_back("Image");
    keys.push_back("IDs");
    std::vector<string> v = get_vector_from_json(j, keys);
    for (auto val: v) {
        cout << val << endl;
    }

    cout << "ok" << endl;

}
