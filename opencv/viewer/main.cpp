/*
This is a demo application to showcase how you can use an
interaction area (iarea). An iarea can be used to track mouse
interactions over an specific space.

Copyright (c) 2016 Fernando Bevilacqua <dovyski@gmail.com>
Licensed under the MIT license.
*/

#include <iostream>
#include <stdio.h>
#include <stdint.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME "Viewer to get depth"

#define IMAGE_FILE          "../depth1540976937.png"
#define BIN_FILE            "../depth1540976937.bin"
#define DEFAULT_WIDTH       640
#define DEFAULT_HEIGHT      480

bool load_bin_to_buffer(const char* fn, uint8_t* buffer, size_t buffer_size)
{
    FILE* fptr = fopen(fn, "rb");
    if (fptr == NULL) {
        return false;
    }
    size_t rs = fread(buffer, sizeof(uint8_t), buffer_size, fptr);
    //printf("rs: %d\n", (int)rs);
    (void)rs;
    fclose(fptr);
    return true;
}

int get_dpeth_pt(uint8_t* buffer, int x, int y)
{
    int res = 0;
    if ( (x < 0 || x >= DEFAULT_WIDTH) || (y < 0 || y >= DEFAULT_HEIGHT) ) {
        printf("%s: OOB %d,%d\n", __func__, x, y);
        return res;
    }

    uint16_t* pt = (uint16_t*)buffer;
    int cnt = y * DEFAULT_WIDTH + x;
    res = (int)*(pt+cnt);

    if (false)
        printf("res: %04x\n", res);

    return res;
}

int action(const std::string& binfn, const std::string& imgfn)
{
    //cv::Mat frame = cv::Mat(DEFAULT_HEIGHT, DEFAULT_WIDTH, CV_8UC3);
    cv::Mat frame = cv::imread(imgfn);
    size_t buffer_size = DEFAULT_HEIGHT * DEFAULT_WIDTH * 2;
    uint8_t buffer[buffer_size];
    if (!load_bin_to_buffer(binfn.c_str(), buffer, buffer_size)) {
        printf("failed to load: %s\n", binfn.c_str());
    }

    // Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
    cvui::init(WINDOW_NAME);

    while (true) {
        // Fill the frame with a nice color
        //frame = cv::Scalar(49, 52, 49);

        // Render a rectangle on the screen.
        //cv::Rect rectangle(230, 40, 240, 50);
        //cvui::rect(frame, rectangle.x, rectangle.y, rectangle.width, rectangle.height, 0x00ff0000);
        cv::Point pt1(230, 40);
        cv::Point pt2(450, 90);
        cv::rectangle(frame, pt1, pt2, cv::Scalar(49, 52, 49), cv::FILLED);
#if 0
        // Check what is the current status of the mouse cursor
        // regarding the previously rendered rectangle.
        int status = cvui::iarea(rectangle.x, rectangle.y, rectangle.width, rectangle.height);

        // cvui::iarea() will return the current mouse status:
        //  CLICK: mouse just clicked the interaction are
        //  DOWN: mouse button was pressed on the interaction area, but not released yet.
        //  OVER: mouse cursor is over the interaction area
        //  OUT: mouse cursor is outside the interaction area
        switch (status) {
            case cvui::CLICK:   std::cout << "Rectangle was clicked!" << std::endl; break;
            case cvui::DOWN:    cvui::printf(frame, 240, 70, "Mouse is: DOWN"); break;
            case cvui::OVER:    cvui::printf(frame, 240, 70, "Mouse is: OVER"); break;
            case cvui::OUT:     cvui::printf(frame, 240, 70, "Mouse is: OUT"); break;
        }
#endif
        // Show the coordinates of the mouse pointer on the screen
        int x = cvui::mouse().x;
        int y = cvui::mouse().y;
        cvui::printf(frame, 240, 50, "Mouse pointer is at (%d,%d)", x, y);
        cvui::printf(frame, 240, 70, "Depth is %4d (mm)", get_dpeth_pt(buffer, x, y));

        // This function must be called *AFTER* all UI components. It does
        // all the behind the scenes magic to handle mouse clicks, etc.
        cvui::update();

        // Show everything on the screen
        cv::imshow(WINDOW_NAME, frame);

        // Check if ESC key was pressed
        if (cv::waitKey(50) == 27) {
            printf("break\n");
            break;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc <= 1) {
        printf("please specify the file name of depth raw data and image for viewer background (depth or color)\n");
        printf("\t./viewer depth.bin depth.png\n");
        printf("--- OR ---\n\t./viewer depth.bin color.png\n");
    } else {
        std::string binfn = argv[1];
        std::string imgfn = argv[2];
        action(binfn, imgfn);
    }
    return 0;
}
