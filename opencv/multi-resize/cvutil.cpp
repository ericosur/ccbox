#include "cvutil.h"
#include <iostream>

/**
    refer from;
    https://docs.opencv.org/2.4/modules/imgproc/doc/geometric_transformations.html?highlight=resize

    INTER_NEAREST - a nearest-neighbor interpolation
    INTER_LINEAR - a bilinear interpolation (used by default)
    INTER_AREA - resampling using pixel area relation. It may be a preferred method for image decimation, as it gives moire’-free results. But when the image is zoomed, it is similar to the INTER_NEAREST method.
    INTER_CUBIC - a bicubic interpolation over 4x4 pixel neighborhood
    INTER_LANCZOS4 - a Lanczos interpolation over 8x8 pixel neighborhood
 */
void resize_file(const std::string& fn)
{
    using namespace std;
    using namespace cv;

    const double RESIZE_FACTOR = 0.5;

    Mat src = imread(fn);
    Mat dst[5];

    // specify fx and fy and let the function compute the destination image size.
    resize(src, dst[0], Size(), RESIZE_FACTOR, RESIZE_FACTOR, INTER_NEAREST);
    resize(src, dst[1], Size(), RESIZE_FACTOR, RESIZE_FACTOR, INTER_LINEAR);
    resize(src, dst[2], Size(), RESIZE_FACTOR, RESIZE_FACTOR, INTER_AREA);
    resize(src, dst[3], Size(), RESIZE_FACTOR, RESIZE_FACTOR, INTER_CUBIC);
    resize(src, dst[4], Size(), RESIZE_FACTOR, RESIZE_FACTOR, INTER_LANCZOS4);
}
