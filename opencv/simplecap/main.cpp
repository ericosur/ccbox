#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#define FRAME_WIDTH    640
#define FRAME_HEIGHT   480
#define VIDEO_WIN      "video"

int test()
{
    using namespace cv;
    int vid_idx = 0;

    namedWindow(VIDEO_WIN, WINDOW_AUTOSIZE);
    moveWindow(VIDEO_WIN, 50, 50);

    VideoCapture cap(vid_idx); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    //set height and width of capture frame
    cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

    Mat cameraFeed;
    printf("press ESC or 'q' to quit %s...\n", __func__);
    while (true) {
        //store image to matrix
        cap.read(cameraFeed);
        imshow(VIDEO_WIN, cameraFeed);
        int key = waitKey(1);
        if (key == 27 || key == 'q') {
            break;
        }
    }
    destroyAllWindows();
    return 0;
}

int main(int argc, char *argv[])
{
    test();
    return 0;
}
