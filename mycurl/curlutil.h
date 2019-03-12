#ifndef __UTIL_CURLUTIL_H__
#define __UTIL_CURLUTIL_H__

#include <string>
#define CURL_STATICLIB
#include <curl/curl.h>


void read_json_from_file(const std::string& fn);
void read_json_from_file(const char* fname);
CURLcode query_via_curl(const std::string& query_string, const std::string& json_fn);


#endif  // __UTIL_CURLUTIL_H__
