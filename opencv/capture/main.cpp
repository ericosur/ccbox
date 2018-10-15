#include "readsetting.h"
#include "cvutil.h"

#include <iostream>

int demoCapture();
int demoTest();
int test_realsense();


int main(int argc, char *argv[])
{
#if 0
    std::cout << "call test_iou()\n";
    cvutil::test_iou();
#endif

#if 1

    ReadSetting* settings = ReadSetting::getInstance();

#ifdef USE_REALSENSE
    if (settings->use_realsense) {
        test_realsense();
    }
#endif

#endif

#if 0
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
