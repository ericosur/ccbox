#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

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
    cap.set(CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cap.set(CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

    Mat frame;
    printf("press ESC or 'q' to quit %s...\n", __func__);
    printf("press 's' to save image...\n");
    int cnt = 0;
    char fn_buffer[40];
    while (true) {
        //store image to matrix
        cap.read(frame);
        imshow(VIDEO_WIN, frame);
        int key = waitKey(1);
        if (key == 27 || key == 'q') {
            break;
        } else if (key == 's') {
            sprintf(fn_buffer, "saved%04d.jpg", cnt);
            imwrite(fn_buffer, frame);
            printf("%s saved\n", fn_buffer);
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
