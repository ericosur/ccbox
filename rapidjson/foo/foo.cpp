#include <pbox/pbox.h>

#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/encodings.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"

using namespace rapidjson;
using namespace std;

#define DATAPATH    "src/ccbox/data/"
#define JSONFILE    "test.json"
#define TEST_OUTPUT_JSON    "/tmp/zzz.json"

typedef char byte;
const int BUFFER_SIZE = 4096;


void show_banner(const string& s)
{
    cout << "***** " << s << " *****\n";
}

// returns the path to settings
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
    cout << __func__ << ": " << doc["description"]["name"].GetString() << endl;
}

bool read_stream(const string& fn, Document& d)
{
    FILE* fp = fopen(fn.c_str(), "rb"); // non-Windows use "r"
    if (fp == NULL) {
        return false;
    }
    char readBuffer[BUFFER_SIZE];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    d.ParseStream(is);
    fclose(fp);
    return true;
}

bool write_stream(const string& fn, const Document& d)
{
    FILE* fp = fopen(fn.c_str(), "wb");
    if (fp == NULL) {
        return false;
    }
    bool putBOM = false;
    const UTFType useType = kUTF8;
    char buffer[BUFFER_SIZE];
    FileWriteStream os(fp, buffer, BUFFER_SIZE);

    typedef AutoUTFOutputStream<unsigned, FileWriteStream> OutputStream;
    OutputStream eos(os, useType, putBOM);
    PrettyWriter<OutputStream, UTF8<>, AutoUTF<unsigned> > writer(eos);
    //Writer<FileWriteStream> writer(os);

    d.Accept(writer);
    fclose(fp);
    return true;
}

bool read_file_and_get_doc(const string& fn, Document& d)
{
    byte buffer[BUFFER_SIZE];
    if ( read_file(fn.c_str(), buffer) ) {
        //cerr << "buffer: " << buffer << endl;
        d.Parse(buffer);
        if (d.IsObject()) {
            return true;
        }

    } else {
        cerr << "error reading\n";
    }
    return false;
}

void show_array(const Document& d)
{
    if ( d.HasMember("extra") ) {
        if ( d["extra"].IsArray() ) {
            cerr << "extra array ok\n";
            const Value& a = d["extra"];
            for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr) {
                const Value& o = (*itr);
                if ( o["lang"].IsString() ) {
                    cout << "lang: " << o["lang"].GetString()
                        << "data: " << o["data"].GetString()
                        << "\n";
                } else {
                    cout << "encounter null\n";
                }

            }
        } else {
            cerr << "extra array nok\n";
        }
    } else {
        cerr << "not such section\n";
    }
}

void test_read(const string& fn)
{
    Document d;

    show_banner(__func__);
    cout << "method #1\n";
    read_stream(fn, d);
    show_array(d);

    cout << "method #2\n";
    read_file_and_get_doc(fn, d);
    show_value(d);

}

void test_write()
{
    show_banner(__func__);
    // assign a raw string
    cout << "test output json...\n";
    char str[] = R"(
        {
            "name": "writer_test",
            "description": "a test json for writer test",
            "result": {
                "name": "test1",
                "score": 100,
                "ustring": "中文輸入法"
            }
        }
    )";
    // parse as json document
    Document d;
    d.Parse(str);
    // output one data member
    if ( d.HasMember("result") ) {
        cout << "result.ustring: " << d["result"]["ustring"].GetString() << "\n";
    }
    write_stream(TEST_OUTPUT_JSON, d);

}

void test()
{
    std::string fn = get_jsonfile();
    cout << "read value from: " << fn << "\n";
    assert(pbox::is_file_exist(fn));

    test_read(fn);
    test_write();

}
