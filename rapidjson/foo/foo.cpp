#include <pbox/pbox.h>

#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

using namespace rapidjson;
using namespace std;

#define DATAPATH    "src/ccbox/data/"
#define JSONFILE    "test.json"

typedef char byte;
const int BUFFER_SIZE = 4096;

std::string get_jsonfile()
{
    string json_file = pbox::get_home() + '/' + DATAPATH + JSONFILE;
    return json_file;
}


bool read_file(const char* fn, byte* buffer)
{
    FILE *fp = fopen(fn, "r");
    if (fp == NULL) {
        fprintf(stderr, "could not read file: %s\n", fn);
        return false;
    }

    size_t readsize = 0;
    while (!feof(fp)) {
        readsize += fread(buffer, sizeof(byte), BUFFER_SIZE, fp);
    }
    fclose(fp);

    return true;
}

void show_value(const Document& doc)
{
    cout << doc["description"]["name"].GetString() << endl;
}

void read_stream(const string& fn)
{
    FILE* fp = fopen(fn.c_str(), "rb"); // non-Windows use "r"
    char readBuffer[BUFFER_SIZE];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document d;
    d.ParseStream(is);
    fclose(fp);
    show_value(d);
}

void get_description(const string& fn)
{
    byte buffer[BUFFER_SIZE];
    if ( read_file(fn.c_str(), buffer) ) {
        //cerr << "buffer: " << buffer << endl;
        Document doc;
        doc.Parse(buffer);
        assert(doc.IsObject());
        show_value(doc);
    } else {
        cerr << "error reading\n";
        return ;
    }

}

void test()
{
    std::string fn = get_jsonfile();
    read_stream(fn);
    get_description(fn);
}
