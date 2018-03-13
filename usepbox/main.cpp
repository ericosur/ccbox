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

bool load_rgbbin_to_buffer(const char* fn, uint8_t* buffer, size_t buffer_size)
{
    FILE* fptr = fopen(fn, "rb");
    if (fptr == NULL) {
        return false;
    }
    size_t rs = fread(buffer, sizeof(uint8_t), buffer_size, fptr);
    printf("rs: %d\n", (int)rs);
    fclose(fptr);
    return true;
}

bool load_depthbin_to_buffer(const char* fn, uint8_t* buffer, size_t buffer_size)
{
    FILE* fptr = fopen(fn, "rb");
    if (fptr == NULL) {
        return false;
    }
    size_t rs = fread(buffer, sizeof(uint8_t), buffer_size, fptr);
    (void)rs;
    fclose(fptr);
    return true;
}

uint16_t get_dpeth_pt(uint16_t* buffer, int x, int y)
{
    uint16_t pt = 0;
    for (int j = 0; j < y; j++) {
        for (int i = 0; i < x; i++) {
            pt = *buffer ++;
        }
    }
    return pt;
}

void test_read_bin()
{
#ifdef USE_TESTDIST
    using namespace cv;

    const int max_width = 640;
    const int max_height = 480;
    const int rgb_byte = 3;
    const int depth_byte = 2;
    const int max_str_len = 256;

    char rgbfn[max_str_len];
    char depfn[max_str_len];

    size_t buffer_size =  max_width*max_height*rgb_byte;
    uint8_t rgb_buffer[buffer_size];
    size_t dep_buffer_size = max_width*max_height*depth_byte;
    uint8_t dep_buffer[dep_buffer_size];
    const char dir[] = "/home/rasmus/realcam";
    for (int i = 0; i<6; ++i) {
        snprintf(rgbfn, max_str_len, "%s/rgb%d.bin", dir, i);
        snprintf(depfn, max_str_len, "%s/depth%d.bin", dir, i);
        load_rgbbin_to_buffer(rgbfn, rgb_buffer, buffer_size);
        load_depthbin_to_buffer(depfn, dep_buffer, dep_buffer_size);

        cv::Mat cimg(cv::Size(max_width, max_height), CV_8UC3, (void*)rgb_buffer);

        for (int ii=0; ii<10; ++ii) {
            int x=ii*10;
            int y=100;
            printf("depth: %d, %d, %d\n", x, y, (int)get_dpeth_pt((uint16_t*)dep_buffer, x, y));
        }

        cv::cvtColor(cimg, cimg, cv::COLOR_RGB2BGR);
        cv::imshow("read", cimg);
        cv::namedWindow("read", WINDOW_AUTOSIZE);
        cv::waitKey(0);
    }
#endif  // USE_TESTDIST
}


void show_features()
{
    printf("demo features:\n");
#ifdef USE_DEMO1
    printf("USE_DEMO1\n");
#endif
#ifdef USE_DEMO2
    printf("USE_DEMO2\n");
#endif
#ifdef USE_REALSENSE
    printf("USE_REALSENSE\n");
#endif
#ifdef USE_TESTDIST
    printf("USE_TESTDIST\n");
#endif
}

int main()
{
    test_get_version();
    show_features();

    demo1();
    demo2();

    test_read_bin();


#if 0
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
#endif

    return 0;
}
