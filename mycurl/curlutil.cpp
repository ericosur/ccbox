#include "curlutil.h"

#include <algorithm>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

struct MemoryStruct {
  char *memory;
  size_t size;
};

using namespace std;
using namespace nlohmann;

void show_json_elements(const json& jj)
{
    std::cout << std::setw(4) << jj << std::endl;
}

void read_json_from_memory(const char* ptr)
{
    try {
        json jj = json::parse(ptr);
        show_json_elements(jj);
    } catch (json::parse_error& e) {
        cout << "parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
       std::cout << "out of range:" << e.what() << '\n';
    }
}

void read_json_from_file(const std::string& fn)
{
    read_json_from_file(fn.c_str());
}

void read_json_from_file(const char* fname)
{
    try {
        ifstream inf(fname);
        json jj;
        inf >> jj;
        show_json_elements(jj);

    } catch (json::parse_error& e) {
        cout << "parse error:" << e.what() << endl;
    } catch (json::out_of_range& e) {
       std::cout << "out of range:" << e.what() << '\n';
    }
}

void write_chunk_to_my_file(MemoryStruct* pChunk, const char* fname)
{
    FILE* fptr = NULL;
    if ( (fptr = fopen(fname, "wb")) == NULL ) {
        fprintf(stderr, "failed to open file to write\n");
        return;
    }
    size_t byte_wrote = fwrite(pChunk->memory, sizeof(char), pChunk->size, fptr);
    if (byte_wrote != pChunk->size) {
        fprintf(stderr, "something wrong, wrote %ld but request %ld\n",
                byte_wrote, pChunk->size);
    }
    fclose(fptr);
    printf("output json to: %s\n", fname);
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
       /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}



CURLcode query_via_curl(const std::string& query_string, const std::string& json_fn)
{
    CURL *curl = NULL;
    CURLcode res = CURLE_OK;

    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl) {
        fprintf(stderr, "[DEBUG] query_string: %s\n", query_string.c_str());

        //curl_easy_setopt(curl, CURLOPT_URL, "https://google.com");
        curl_easy_setopt(curl, CURLOPT_URL, query_string.c_str());
        /* could be redirected, so we tell LibCurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        // setup a write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        /* some servers don't like requests that are made without a user-agent
         field, so we provide one */
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "[ERR] curl_easy_perform() failed: %s (%d)\n", curl_easy_strerror(res), res);
        } else {
            /*
             * Now, our chunk.memory points to a memory block that is chunk.size
             * bytes big and contains the remote file.
             *
             * Do something nice with it!
             */
            //printf("%lu bytes retrieved\n", (long)chunk.size);
            write_chunk_to_my_file(&chunk, json_fn.c_str());
            read_json_from_memory(chunk.memory);
        }
    }

    /* Always cleanup */
    curl_easy_cleanup(curl);
    free(chunk.memory);
    curl_global_cleanup();

    return res;
}
