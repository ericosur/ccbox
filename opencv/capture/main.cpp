#include "readsetting.h"
#include "cvutil.h"

#include <iostream>

int demoCapture();
int demoTest();
int test_realsense();


int main(int argc, char *argv[])
{
    std::cout << "call test_iou()\n";
    cvutil::test_iou();

#ifdef USE_REALSENSE
    ReadSetting *settings = ReadSetting::getInstance();
    if (settings->use_realsense) {
        test_realsense();
    } else {
        demoTest();
    }
#else
    ReadSetting *settings = ReadSetting::getInstance();
    if (settings->use_edge_test) {
        if ( demoTest() == -1 ) {
            printf("edge test failed...\n");
        }
    }

    if (settings->use_hsv_test) {
        demoCapture();
    }
#endif

    return 0;
}
