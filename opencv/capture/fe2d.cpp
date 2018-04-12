#include <opencv2/core/core.hpp>
//core/types.hpp
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int test()
{
    vector<Mat> imgs;

    imgs.push_back( imread("outdir/capt0000.png") );
    imgs.push_back( imread("outdir/capt0001.png") );

    // vector of keyPoints
    vector<vector<KeyPoint>> keypoints;

    // construction of the fast feature detector object
    Ptr<FeatureDetector> detector = FastFeatureDetector::create(50);

    detector->detect(imgs, keypoints);

    for (size_t ii = 0; ii < keypoints.size(); ii++) {
        const vector<KeyPoint>& kp = keypoints[ii];
        cout << "ii:" << ii << ", " << "sz: " << kp.size() << endl;
        for (size_t jj = 0; jj < kp.size(); jj++) {
            const KeyPoint& pt = kp[jj];
            if (jj) {
                circle(imgs[0], Point(pt.pt.x, pt.pt.y), 6, Scalar(255,0,0,255));
            } else {
                circle(imgs[0], Point(pt.pt.x, pt.pt.y), 6, Scalar(255,255,255,255));
            }
        }
    }


    // for ( unsigned int i = 0; i < sz; i++ ) {
    //     const KeyPoint& kp1 = v1[i];
    //     const KeyPoint& kp2 = v2[i];

    //     if (kp1 == kp2) {
    //         circle(img1, Point(kp1.pt.x, kp1.pt.y), 8, Scalar(255,255,255,255));
    //     } else {
    //         circle(img1, Point(kp2.pt.x, kp2.pt.y), 8, Scalar(255,255,255,255));
    //     }
    // }

    imshow("FAST feature", imgs[0]);
    cvWaitKey(0);

    return 0;
}

int main()
{
    test();
    return 0;
}