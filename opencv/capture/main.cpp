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
int test_from_image(const std::string& fn);

int main(int argc, char *argv[])
{
    using namespace std;

    ReadSetting *settings = ReadSetting::getInstance();

#ifdef USE_REALSENSE
    if (miscutil::handleOpt(argc, argv)) {
        cout << "arguments handled\n";
    } else if (!settings->isOpened) {
        if (!settings->read_setting()) {
            cout << "[ERROR] cannot open config\n";
            exit(1);
        }
    }

    if (settings->input_image != "") {
        test_from_image(settings->input_image);
    } else {
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
