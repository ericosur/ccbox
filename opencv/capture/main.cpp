#include "readsetting.h"
#include "cvutil.h"
#include "miscutil.h"
#include <string>
#include <iostream>
#include <unistd.h>

#define SLEEP_LENGTH    (3*1000*1000)

int demoCapture();
int demoTest();
int test_realsense();
int show_image(const std::string& fn);
int test_from_image();

int main(int argc, char *argv[])
{
    using namespace std;

    ReadSetting *sett = ReadSetting::getInstance();

#ifdef USE_REALSENSE
    if (miscutil::handleOpt(argc, argv)) {
        cout << "[INFO] arguments handled\n";
    }

    if (!sett->read_setting()) {
        cout << "[ERROR] cannot open config\n";
        exit(1);
    }
    if (sett->input_image != "") {
        show_image(sett->input_image);
    }
    else if (sett->color_image != "" && sett->depth_image != "") {
        test_from_image();
    }
    else {
        test_realsense();
    }
#else
    // cout << "call test_iou()\n";
    // cvutil::test_iou();
    cout << "using local webcam to capture...\n";

    if (settings->use_edge_test) {
        cout << "edge test...\n";
        if ( demoTest() == -1 ) {
            cout << "demoTest() failed\n";
        }
        usleep(SLEEP_LENGTH);
    }

    if (settings->use_hsv_test) {
        cout << "hsv test...\n";
        demoCapture();
    }
#endif

    return 0;
}
