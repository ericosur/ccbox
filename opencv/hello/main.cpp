#include <opencv2/core.hpp>

int hello_cv2()
{
    printf("OpenCV version: %d.%d.%d\n", CV_MAJOR_VERSION, CV_VERSION_MINOR, CV_VERSION_REVISION);
    return 0;
}

int main(int argc, char *argv[])
{
    hello_cv2();
    return 0;
}
