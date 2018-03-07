#include "pbox.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <assert.h>

#ifdef USE_REALSENSE
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

#define DATAPATH "src/ccbox/data/"
#define JSONFILE "usepbox.json"

void test();
void test_get_version();


std::string get_jsonpath()
{
    std::string jsonfile = pbox::get_home() + "/" + DATAPATH + JSONFILE;
    if (pbox::is_file_exist(jsonfile)) {
        return jsonfile;
    } else {
        std::cerr << "file not found: " << jsonfile << std::endl;
        assert(0);
        return "";
    }
}

#ifdef USE_TESTDIST

const int DEFAULT_WIDTH = 640;
const int DEFAULT_HEIGHT = 480;

bool read_raw_from_bin(const char* fn, uint16_t* buffer, int w, int h);
float get_dist_from_raw(uint16_t* buffer, int w, int h, int x, int y);

std::string get_distsetting()
{
    std::string jsonfile = pbox::get_home() + "/src/ccbox/usepbox/setting.json";
    if (pbox::is_file_exist(jsonfile)) {
        return jsonfile;
    } else {
        std::cerr << "file not found: " << jsonfile << std::endl;
        assert(0);
        return "";
    }
}

void test_dist_func()
{
    using namespace std;
    string jsonfile = get_distsetting();
    string path = pbox::get_string_from_jsonfile(jsonfile, "path");
    string answer = pbox::get_string_from_jsonfile(jsonfile, "answer");
    string pattern = pbox::get_string_from_jsonfile(jsonfile, "pattern");
    int number = pbox::get_int_from_jsonfile(jsonfile, "number");
    cout << "path:" << path << endl;
    cout << "answer:" << answer << endl;
    cout << "pattern:" << pattern << endl;
    cout << "number:" << number << endl;

#if 1
    uint16_t buffer[DEFAULT_WIDTH*DEFAULT_HEIGHT];
    char rawfn[DEFAULT_HEIGHT];
    char fn[DEFAULT_HEIGHT];
    string answer_json = path + "/" + answer;
    char key[20];

    for (int idx = 0; idx < number; ++idx) {
        snprintf(rawfn, DEFAULT_HEIGHT, pattern.c_str(), idx);
        snprintf(fn, DEFAULT_HEIGHT, "%s/%s", path.c_str(), rawfn);
        //printf("fn: %s\n", fn);

        read_raw_from_bin(fn, (uint16_t*)buffer, DEFAULT_WIDTH, DEFAULT_HEIGHT);
        float rdist = get_dist_from_raw((uint16_t*)buffer, DEFAULT_WIDTH, DEFAULT_HEIGHT,
            DEFAULT_WIDTH/2, DEFAULT_HEIGHT/2);

        printf("rdist: %.3f\n", rdist);
        snprintf(key, sizeof(key), "%d", idx);
        double ans = pbox::get_double_from_jsonfile(answer_json, key);
        printf("ans = %.3f\n", ans);
    }

#endif
}

#endif  // USE_TESTDIST

void mytest(const std::vector<std::string>& k)
{
    using namespace std;

    vector<string> strings;
    strings = pbox::get_vector_from_jsonfile(get_jsonpath(), k);
    cout << "test:" << endl;
#if __cplusplus >= 201103L
    for (string s : strings) {
        cout << s << endl;
    }
#else
    for (vector<string>::const_iterator i = strings.begin(); i != strings.end(); ++i) {
        cout << *i << endl;
    }
#endif
}

void print_banner(const char* msg)
{
    const int repeat = 20;
    for (int i = 0; i < repeat; ++i) {
        printf("=");
    }
    printf(" %s ", msg);
    for (int i = 0; i < repeat; ++i) {
        printf("=");
    }

    printf("\n");
}

int demo1()
{
#ifdef USE_DEMO1
    using namespace std;

    std::string jsonfile = get_jsonpath();

    print_banner("dump_jsonfile");
    pbox::dump_jsonfile(jsonfile);
    print_banner("get_vector_from_jsonfile");
    vector<string> k;
    // .test
    k.push_back("test");
    mytest(k);

    print_banner("get_vector_from_jsonfile");
    // .query.result.item
    k.clear();
    k.push_back("query");
    k.push_back("result");
    k.push_back("item");
    mytest(k);

    print_banner("get_double_from_jsonfile");
    // .values.pi
    k.clear();
    k.push_back("values");
    k.push_back("pi");
    printf("double from .values.pi: %lf\n",
           pbox::get_double_from_jsonfile(jsonfile, k));
#endif
    return 0;
}

void demo2()
{
    //test();

#ifdef USE_DEMO2
    using namespace std;
    vector<string> k;

    print_banner("get_double_from_jsonfile");
    // .values.pi
    k.clear();
    k.push_back("values");
    k.push_back("pi");
    double dd;
    dd = pbox::get_double_from_jsonfile(get_jsonpath(), k);
    cout << dd << endl;

    print_banner("get_string_from_jsonfile");
    string val = pbox::get_string_from_jsonfile(get_jsonpath(), "name");
    cout << "name: " << val << endl;
    val = pbox::get_string_from_jsonfile(get_jsonpath(), "comment");
    cout << "comment: " << val << endl;

    double pp = pbox::get_double_from_jsonfile(get_jsonpath(), "possibility");
    cout << "possibility: " << pp << endl;

#if 0   // still buggy
    print_banner("get_value_from_jsonfile");
    double p;
    if ( pbox::get_value_from_jsonfile(JSONFILE, "possibility", p) ) {
        cout << "possibility: " << p << "\n";
    }
#endif

#endif  // USE_DEMO2


}

#ifdef USE_REALSENSE
void get_image_and_show()
{
    cv::Mat img;
    pbox::get_color_mat_from_realsense(img);
    cv::imshow("fuck", img);
}
#endif

int main()
{
    test_get_version();

    demo1();
    demo2();

#ifdef USE_REALSENSE
    // const int maxx = 640;
    // const int maxy = 480;
    pbox::init_realsense();
    while (true) {
        get_image_and_show();
        if ( cv::waitKey(1) == 27 ) {
            break;
        }
    }
#endif

#ifdef USE_TESTDIST
    test_dist_func();
#endif  // USE_TESTDIST

    return 0;
}
