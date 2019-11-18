/// fix depth image

#include "cvutil.h"
#include "rsutil.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <vector>
#include <regex>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const bool DO_IMSHOW = false;

bool load_bin_to_buffer(const std::string& fn, uint8_t* buffer, size_t buffer_size)
{
    FILE* fptr = fopen(fn.c_str(), "rb");
    if (fptr == NULL) {
        return false;
    }
    size_t rs = fread(buffer, sizeof(uint8_t), buffer_size, fptr);
    //printf("rs: %d\n", (int)rs);
    (void)rs;
    fclose(fptr);
    return true;
}

bool fix_buffer(const int width, const int height, uint16_t* buffer, cv::Mat& img)
{
    const uint16_t low = 300;
    const uint16_t high = 1500;
    //const float range = (float)(high - low);
    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; ++y) {
        auto dpi = y * width;
        cv::Vec3b* p = img.ptr<cv::Vec3b>(y);
        for (int x = 0; x < width; ++x, ++dpi) {
            uint16_t di = buffer[dpi];
            if (di <= low || di >= high) {
                p[x] = cv::Vec3b(0, 0, 0);
            } else {
                // do nothing
            }
        }
    }
    if (DO_IMSHOW)
        imshow("out", img);
    return true;
}

bool retrieve_filename(const string& s, string& imgf,
    string& depf, string& outf)
{
    std::regex word_regex("(\\d+)");
    auto words_begin = std::sregex_iterator(s.begin(), s.end(), word_regex);
    auto words_end = std::sregex_iterator();
    string ans;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        if (match_str.size()) {
            ans = match_str;
            //cout << "ans: " << match_str << endl;
            break;
        }
    }
    imgf = "depth" + ans + ".png";
    depf = "depth" + ans + ".bin";
    outf = "rmbg" + ans + ".png";

    return true;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        cout << "need 3 parameters, given "
            << "imgf, depf, outf" << endl;
        return -1;
    }
    string imgf = argv[1];
    string depf = argv[2];
    string outf = argv[3];

    cout << "imgf: " << imgf << endl;
    cout << "depf: " << depf << endl;

    const uint16_t width = 640;
    const uint16_t height = 360;
    const size_t buffer_size = width * height * 2;
    uint16_t buffer[buffer_size] = {0};

    if ( load_bin_to_buffer(depf, (uint8_t*)buffer, buffer_size) ) {
        cout << "load raw depth ok\n";
    } else {
        cout << "load raw depth failed\n";
        exit(1);
    }

    Mat depth_img = imread(imgf);
    if (depth_img.data == nullptr) {
        cout << "load image failed" << endl;
        exit(1);
    }
    if (DO_IMSHOW)
        imshow("show", depth_img);

    fix_buffer(width, height, buffer, depth_img);
    cout << "outf: " << outf << endl;
    imwrite(outf, depth_img);

    if (DO_IMSHOW) {
        int key = waitKey(0);
        if (key == 0x1B) {
            cout << "user break" << endl;
        }
        destroyAllWindows();
    }
    return 0;
}
