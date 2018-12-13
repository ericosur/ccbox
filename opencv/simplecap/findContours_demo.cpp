/**
 * @function findContours_Demo.cpp
 * @brief Demo code to find contours in an image
 * @author OpenCV team
 */
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat src;
Mat src_gray;
int thresh = 100;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

#define SOURCE_WIN    "source"
#define CONTOUR_WIN   "contours"

void init_windows()
{
    namedWindow(SOURCE_WIN, WINDOW_NORMAL);
    moveWindow(SOURCE_WIN, 0, 0);
    resizeWindow(SOURCE_WIN, 444, 273);
    namedWindow(CONTOUR_WIN, WINDOW_NORMAL);
    moveWindow(CONTOUR_WIN, 0, 500);
    resizeWindow(CONTOUR_WIN, 444, 273);
}

/**
 * @function main
 */
int main( int argc, char** argv )
{
    /// Load source image
    CommandLineParser parser( argc, argv, "{@input | ../data/HappyFish.jpg | input image}" );
    src = imread( parser.get<String>( "@input" ) );
    if( src.empty() )
    {
      cout << "Could not open or find the image!\n" << endl;
      cout << "Usage: " << argv[0] << " <Input image>" << endl;
      return -1;
    }

    init_windows();

    /// Convert image to gray and blur it
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    blur( src_gray, src_gray, Size(3,3) );

    /// Create Window
    imshow( SOURCE_WIN, src );

    const int max_thresh = 255;
    createTrackbar( "Canny thresh:", SOURCE_WIN, &thresh, max_thresh, thresh_callback );
    thresh_callback( 0, 0 );

    waitKey();
    return 0;
}

/**
 * @function thresh_callback
 */
void thresh_callback(int, void* )
{
    /// Detect edges using Canny
    Mat canny_output;
    Canny( src_gray, canny_output, thresh, thresh*2 );

    /// Find contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );

    /// Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
    }

    Mat combined;
    cout << src.rows << ", " << src.cols << endl;
    cout << drawing.rows << ", " << drawing.cols << endl;
    hconcat(src, drawing, combined);
    imshow(CONTOUR_WIN, combined);
    /// Show in a window
    //imshow(CONTOUR_WIN, drawing);

}
