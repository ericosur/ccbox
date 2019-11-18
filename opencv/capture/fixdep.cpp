/// fix depth image

#include "cvutil.h"
#include "rsutil.h"

#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

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

bool fix_buffer(uint16_t* buffer, const int width, const int height)
{
    const uint16_t low = 300;
    const uint16_t high = 1500;
    const float range = (float)(high - low);
    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; ++y) {
        auto dpi = y * width;
        for (int x = 0; x < width; ++x, ++dpi) {
            auto di = buffer[dpi];
            if (di < low || di > high) {
                buffer[dpi] = 0;
            } else {
                float v = (((float)di - (float)low) / range) * 65535;
                buffer[dpi] = (uint16_t)v;
            }
        }
    }
    return true;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        cout << "argc must be 3" << endl;
        return -1;
    }
    string inf = argv[1];
    string outf = argv[2];
    cout << inf << endl;
    cout << outf << endl;

    const uint16_t width = 640;
    const uint16_t height = 360;

    const size_t buffer_size = width * height * 2;
    uint16_t buffer[buffer_size] = {0};

    if ( load_bin_to_buffer(inf, (uint8_t*)buffer, buffer_size) ) {
        cout << "load ok\n";
    } else {
        cout << "load failed\n";
    }

    fix_buffer(buffer, width, height);
    Mat img(Size(width, height), CV_16U, (void*)buffer, Mat::AUTO_STEP);
    Mat img0;
    img.convertTo(img0, CV_8U, 256.0/65536.0);
    imwrite(outf, img0);

    return 0;
}
