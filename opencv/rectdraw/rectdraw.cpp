#include <opencv2/opencv.hpp>

cv::Scalar get_color(int i, int j)
{
    return cv::Scalar(80+i*10, 80+j*12, 20+i+j);
}

void test()
{
    using namespace cv;

    const char WINNAME[] = "result";
    const int colsize = 36;
    const int rowsize = 24;
    const int spacew = 0;
    const int spaceh = 4;

    // Create black empty images
    const int maxx = 480;
    const int maxy = 400;
    const int w = 10;
    const int h = 6;

    Mat image = Mat::zeros( maxy, maxx, CV_8UC3 );
    int startx = 0;
    int starty = 0;

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
            rectangle(image, Point(x, y), Point(x+w, y+h), get_color(i, j), 1, 8);
            imshow(WINNAME, image);
        }
        starty += spaceh;
    }

    waitKey(0);
    destroyAllWindows();
}

int main()
{
    test();
    return 0;
}
