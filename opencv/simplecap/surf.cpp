#include "sett.h"
#include <stdio.h>
#include <iostream>

#include <opencv2/opencv.hpp>

#ifndef HAVE_OPENCV_XFEATURES2D
#error Need OpenCV xfeatures2d
#endif

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace cv;
using namespace cv::xfeatures2d;

const std::string json_fn = "../surfdemo.json";
std::vector<std::string> files;


/* @function main */
int main( int argc, char** argv )
{

    load_json(json_fn, files);

    Mat img_1 = imread( files[0], IMREAD_GRAYSCALE );
    Mat img_2 = imread( files[1], IMREAD_GRAYSCALE );

    if ( !img_1.data || !img_2.data ) {
        std::cout<< " --(!) Error reading images " << std::endl;
        return -1;
    }
    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;
    Ptr<SURF> detector = SURF::create( minHessian );
    std::vector<KeyPoint> keypoints_1, keypoints_2;
    detector->detect( img_1, keypoints_1 );
    detector->detect( img_2, keypoints_2 );
    //-- Draw keypoints
    Mat img_keypoints_1;
    Mat img_keypoints_2;
    drawKeypoints( img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    drawKeypoints( img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

    if (img_keypoints_1.cols == img_keypoints_2.cols && img_keypoints_1.rows == img_keypoints_2.rows) {
        //-- Show detected (drawn) keypoints
        Mat combined;
        hconcat(img_keypoints_1, img_keypoints_2, combined);
        imshow("Keypoints", combined);
    } else {
        imshow("img1", img_keypoints_1);
        imshow("img2", img_keypoints_2);
    }

    waitKey(0);

    destroyAllWindows();

    return 0;
}
