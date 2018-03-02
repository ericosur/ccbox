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

#define USE_DEMO1
#define USE_DEMO2

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

    print_banner("get_value_from_jsonfile");
    // .values.pi
    k.clear();
    k.push_back("values");
    k.push_back("pi");
    double dd;
    // if ( get_value_from_jsonfile(JSONFILE, k, dd) ) {
    //     printf("float from .values.pi: %lf\n", dd);
    // } else {
    //     printf("failed to read value\n");
    // }
    dd = pbox::get_double_from_jsonfile(JSONFILE, k);
    cout << dd << endl;

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

    // demo1();
    // demo2();

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

    return 0;
}
