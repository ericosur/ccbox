#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
using namespace cv;

#define WINNAME     "image"

int test()
{
    // Create black empty images
    int maxx = 640;
    int maxy = 480;
    Mat image = Mat::zeros( maxy, maxx, CV_8UC3 );
    int w = 10;
    int h = 6;
    int startx = 0;
    int starty = 0;
    int colsize = 32;
    int rowsize = 32;
    int spacew = 0;
    int spaceh = 4;

    namedWindow(WINNAME, WINDOW_NORMAL);
    moveWindow(WINNAME, 50, 50);
    resizeWindow(WINNAME, maxx, maxy);

    int x = 0;
    int y = 0;
    int i = 0;
    int j = 0;

    for (j=0; j<rowsize; ++j) {
        y = starty + h*j + 5;
        for (i=0; i<colsize; ++i) {
            x = startx + i * (w + spacew);
            rectangle(image, Point(x, y), Point(x+w, y+h), Scalar(80+i*10, 80+j*12, 20+i+j), 1, 8);
            imshow(WINNAME, image);
        }
        starty += spaceh;
    }


    waitKey( 0 );
    return(0);
}

int main()
{
    test();
    return 0;
}