#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

#ifdef HAVE_OPENCV_XFEATURES2D

#include "sett.h"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

const string window_name = "matches";
const string json_fn = "../surfdemo.json";
std::vector<std::string> files;

const char* keys =
    "{ help h |                          | Print help message. }"
    "{ img1 | ../data/box.png          | Path to input image 1. }"
    "{ img2 | ../data/box_in_scene.png | Path to input image 2. }";

void init_windows()
{
    namedWindow(window_name);
    moveWindow(window_name, 0, 0);
    resizeWindow(window_name, 800, 800);
}

int main( int argc, char* argv[] )
{
    CommandLineParser parser( argc, argv, keys );

    if (argc >= 3) {
        files.push_back(parser.get<String>("img1"));
        files.push_back(parser.get<String>("img2"));
    } else {
        load_json(json_fn, files);
    }

    if (!check_files(files)) {
        cout << "[ERROR] failed to load images\n";
        return -1;
    }

    init_windows();

    Mat img1 = imread(files[0], IMREAD_GRAYSCALE);
    Mat img2 = imread(files[1], IMREAD_GRAYSCALE);
    if ( img1.empty() || img2.empty() ) {
        cout << "Could not open or find the image!\n" << endl;
        return -2;
    }

    //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
    //int minHessian = 400;
    int minHessian = 500;
    Ptr<SURF> detector = SURF::create( minHessian );
    std::vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    detector->detectAndCompute( img1, noArray(), keypoints1, descriptors1 );
    detector->detectAndCompute( img2, noArray(), keypoints2, descriptors2 );

    //-- Step 2: Matching descriptor vectors with a brute force matcher
    // Since SURF is a floating-point descriptor NORM_L2 is used
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
    std::vector< DMatch > matches;
    matcher->match( descriptors1, descriptors2, matches );

    //-- Draw matches
    Mat img_matches;
    drawMatches( img1, keypoints1, img2, keypoints2, matches, img_matches );

    //-- Show detected matches
    imshow(window_name, img_matches);

    waitKey();

    return 0;
}
#else
int main()
{
    std::cout << "This tutorial code needs the xfeatures2d contrib module to be run." << std::endl;
    return 0;
}
#endif
