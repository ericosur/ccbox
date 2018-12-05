#include "readsetting.h"
#include "cvutil.h"
#include "miscutil.h"
#include <string>
#include <iostream>

int demoCapture();
int demoTest();
int test_realsense();
int test_from_image(const std::string& fn);

int main(int argc, char *argv[])
{
    using namespace std;

    ReadSetting *settings = ReadSetting::getInstance();

    if (miscutil::handleOpt(argc, argv)) {
        cout << "arguments handled\n";
    } else if (!settings->isOpened) {
        if (!settings->read_setting()) {
            cout << "[ERROR] cannot open config\n";
            exit(1);
        }
    }

#ifdef USE_REALSENSE
    if (settings->input_image != "") {
        test_from_image(settings->input_image);
    } else {
        test_realsense();
    }
#else
    cout << "call test_iou()\n";
    cvutil::test_iou();

    if (settings->use_edge_test) {
        if ( demoTest() == -1 ) {
            cout << "demoTest() failed\n";
        }
    }

    if (settings->use_hsv_test) {
        demoCapture();
    }
#endif

    return 0;
}
