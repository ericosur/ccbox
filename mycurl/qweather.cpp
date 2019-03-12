#include "curlutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <nlohmann/json.hpp>


using namespace std;
using namespace nlohmann;

string remove_quote(const string& input_string)
{
    std::string str1 = input_string;
    str1.erase(std::remove(str1.begin(), str1.end(), '\"'),
               str1.end());
    //std::cout << str1 << '\n';

    return str1;
}

string get_query_string(const string& city)
{
#define QUERY_STR "https://query.yahooapis.com/v1/public/yql?" \
    "q=select+%2A+from+weather.forecast+where+woeid+in+%28+" \
    "select+woeid+from+geo.places+%28+1+%29+where+text+%3D+%22+"
#define QUERY_STR2    "+%22+%29+and+u+%3D+%22+c+%22&format=json"

    string result = string(QUERY_STR) + city + string(QUERY_STR2);
    //cout << "query string:" << result << endl;

    return result;
}

void read_json_elements(const json& jj)
{
    //cout << __func__ << endl;
    try {
        if (jj.at("query").at("results").is_null()) {
            cout << "[ERR] the query is null" << endl;
            return ;
        }

        json foo = jj.at("query").at("results").at("channel");
        cout << "city:" << foo.at("location").at("city") << endl;
        cout << "text:" << foo.at("item").at("condition").at("text") << endl;
        string temp = remove_quote(foo.at("item").at("condition").at("temp"));
        string unit = remove_quote(foo.at("units").at("temperature"));
        string deg = "\xC2\xB0";    // degree symbol
        string result = temp + deg + unit;
        cout << "temp:" << result << endl;
    } catch (json::parse_error& e) {
        cout << "parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
       std::cout << "out of range:" << e.what() << '\n';
    }
}



int query_weather(const string& city)
{
    string query_string = get_query_string(city);
    string fn = "/tmp/foobar.json";

    cout << "query weather for " << city << " =====>" << endl;
    CURLcode res = query_via_curl(query_string, fn);

    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "[ERR] failed to query\n");
    } else {
        fprintf(stderr, "[INFO] output to %s\n", fn.c_str());
        read_json_from_file(fn);
    }

    return 0;
}
