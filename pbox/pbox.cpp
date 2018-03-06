#include "pbox.h"
#include "pbox_internal.h"

#include <fstream>
#include <iostream>
#include "json.hpp"
#include <stdlib.h>
#include <unistd.h>

namespace pbox {

using namespace std;
using namespace nlohmann;

string cached_fn;
json   cached_json;
bool   isCached = false;

std::string get_home()
{
    char* path = getenv("HOME");
    if (path == NULL) {
        return "";
    } else {
        return path;
    }
}

// just a stupid function to check file existence
bool is_file_exist(const char* fname)
{
    if ( access(fname, F_OK) != -1 ) {
        return true;
    } else {
        return false;
    }
}

// overloaded function to use std::string file name
bool is_file_exist(const std::string& fname)
{
    return is_file_exist(fname.c_str());
}

/**
 * dump() is a simple stupid dump function to see buffer in HEX
 * @param buf  [in] buffer to see
 * @param size [in] buffer size
 */
void dump(const char* buf, int size)
{
    for (int i=0; i<size; i++) {
        printf("%02X ", (unsigned char)buf[i]);
        if (i%16==15) {
            printf("\n");
        }
    }
    printf("\n");
}


bool get_json_from_file(const std::string& json_file, json& j)
{
    if ( !is_file_exist(json_file) ) {
        cerr << "file not existed: " << json_file << "\n";
        isCached = false;
        cached_fn = "";
        cached_json.clear();
        return false;
    }

    // file exists, but changed
    if (cached_fn != json_file) {
        isCached = false;
        cached_fn = "";
        cached_json.clear();
    }

    //cout << "read " << json_file << endl;
    try {
        if (!isCached) {
            ifstream inf(json_file);
            inf >> j;
            isCached = true;
            cached_fn = json_file;
            cached_json = j;
        } else {
            // isCached,
            if (cached_fn == json_file) {
                j = cached_json;
            }
        }

        return true;

    } catch (json::parse_error& e) {
        cout << "[get_json_from_file] parse error:" << e.what() << endl;
    }

    return false;
}


template<class T>
bool get_value_from_jsonfile(const std::string& json_file,
                        std::vector<std::string> keys,
                        T& value)
{
    json j;
    if (!get_json_from_file(json_file, j)) {
        return false;
    }

    //cout << "read " << json_file << endl;
    try {
        json kk = j;
        for (string i : keys) {
            kk = kk.at(i);
        }
        value = kk.get<T>();
        return true;
    } catch (json::type_error& e) {
        cout << "[get_value_from_jsonfile] type error:" << e.what() << endl;
    } catch (json::parse_error& e) {
        cout << "[get_value_from_jsonfile] parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
        cout << "[get_value_from_jsonfile] out of range:" << e.what() << endl;
    }
    return false;
}

template<class T>
bool get_value_from_jsonfile(const std::string& json_file,
                             const std::string& key, T& value)
{
    json j;

    if (!get_json_from_file(json_file, j)) {
        return false;
    }

    //cout << "read " << json_file << endl;
    try {
        value = j.at(key).get<T>();
        return true;
    } catch (json::type_error& e) {
        cout << "[get_value_from_jsonfile] type error:" << e.what() << endl;
    } catch (json::parse_error& e) {
        cout << "[get_value_from_jsonfile] parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
        cout << "[get_value_from_jsonfile] out of range:" << e.what() << endl;
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


int get_int_from_json(json& j, vector<string> keys)
{
    int empty = 0;
    try {
        json kk = j;
        for (string i : keys) {
            kk = kk.at(i);
        }
        return kk.get<int>();
    } catch (json::type_error& e) {
        cout << "[get_vector_from_json] type error:" << e.what() << endl;
    } catch (json::parse_error& e) {
        cout << "[get_vector_from_json] parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
        cout << "[get_vector_from_json] out of range:" << e.what() << endl;
    }
    return empty;
}

double get_double_from_json(json& j, vector<string> keys)
{
    double empty = 0.0;
    try {
        json kk = j;
        for (string i : keys) {
            kk = kk.at(i);
        }
        return kk.get<double>();
    } catch (json::type_error& e) {
        cout << "[get_vector_from_json] type error:" << e.what() << endl;
    } catch (json::parse_error& e) {
        cout << "[get_vector_from_json] parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
        cout << "[get_vector_from_json] out of range:" << e.what() << endl;
    }
    return empty;
}

std::string get_string_from_json(const json& j, const std::string& key)
{
    std::string value;
    try {
        value = j.at(key).get<std::string>();
    } catch (json::type_error& e) {
        cout << "[get_vector_from_json] type error:" << e.what() << endl;
    } catch (json::parse_error& e) {
        cout << "[get_vector_from_json] parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
        cout << "[get_vector_from_json] out of range:" << e.what() << endl;
    }
    return value;
}

template<class T>
T get_value_from_json(json& j, vector<string> keys, T& value)
{
    T empty = T();
    try {
        json kk = j;
        for (string i : keys) {
            kk = kk.at(i);
        }
        value = kk.get<T>();
        return value;
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

int get_int_from_jsonfile(const std::string& fn,
                          std::vector<std::string> keys)
{
    json j;
    int empty = 0;
    if (get_json_from_file(fn, j)) {
        return get_int_from_json(j, keys);
    } else {
        return empty;
    }
}

double get_double_from_jsonfile(const std::string& fn,
                                std::vector<std::string> keys)
{
    json j;
    double empty = 0.0;
    if (get_json_from_file(fn, j)) {
        return get_double_from_json(j, keys);
    } else {
        return empty;
    }
}

double get_double_from_jsonfile(const std::string& fn, const std::string& key)
{
    double empty = 0.0;
    try {
        json j;
        if (get_json_from_file(fn, j)) {
            return j.at(key).get<double>();
        }
    } catch (json::type_error& e) {
        cout << "[get_vector_from_json] type error:" << e.what() << endl;
    } catch (json::parse_error& e) {
        cout << "[get_vector_from_json] parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
        cout << "[get_vector_from_json] out of range:" << e.what() << endl;
    }
    return empty;
}


std::string get_string_from_jsonfile(const std::string& fn,
                                     const std::string& key)
{
    json j;
    std::string empty;
    if (get_json_from_file(fn, j)) {
        return get_string_from_json(j, key);
    } else {
        return empty;
    }
}

template<class T>
T get_value_from_jsonfile(const std::string& fn,
                          std::vector<std::string> keys)
{
    json j;
    T empty;
    if (get_json_from_file(fn, j)) {
        return get_value_from_json<T>(j, keys);
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
#if 0
    test_foo();
    cout << endl;
    test_bar();
    cout << endl;
    test_weather();
    test_addon();
#endif
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
        },
        "floating": {
            "pi": 3.141592653589
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
/*
    std::vector<string> ss;
    get_value_from_json(j, keys, ss);
    for (auto val: ss) {
        cout << val << endl;
    }
*/
    double p;
    keys.clear();
    keys.push_back("floating");
    keys.push_back("pi");
    get_value_from_json(j, keys, p);
    cout << "p: " << p << endl;
}


}   // namespace pbox
