#include "readset.h"

int demoCapture();
int demoTest();
int test_realsense();


int main(int argc, char *argv[])
{

#ifdef USE_REALSENSE
    ReadSetting* settings = ReadSetting::getInstance();
    if (settings->use_realsense) {
        test_realsense();
    }
#else
    if ( demoTest() == -1 ) {
        return -1;
    }
#endif

    demoCapture();

    return 0;
}
