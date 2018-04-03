#include "readset.h"

int demoCapture();
int demoTest();
int test_realsense();


int main(int argc, char *argv[])
{

    ReadSetting* settings = ReadSetting::getInstance();

#ifdef USE_REALSENSE
    if (settings->use_realsense) {
        test_realsense();
    }
#endif

    if (settings->use_edge_test) {
        if ( demoTest() == -1 ) {
            printf("edge test failed...\n");
        }
    }

    if (settings->use_hsv_test) {
        demoCapture();
    }


    return 0;
}
