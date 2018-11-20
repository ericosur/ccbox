#include <iostream>
#include <opencv2/opencv.hpp>

const auto TEST_IMAGE = "../../data/banana600.jpg";

int hello_cv2()
{
    printf("OpenCV version: %d.%d.%d\n", CV_MAJOR_VERSION, CV_VERSION_MINOR, CV_VERSION_REVISION);
    return 0;
}

/// [in] img
/// [in] p1, p2
/// [out] points
int iter(const cv::Mat& img, const cv::Point& p1, const cv::Point& p2, std::vector<cv::Point>& points)
{
    using namespace cv;

    LineIterator it(img, p1, p2);

    //cout << "from " << p1 << " to " << p2 << "\t";
    //cout << "it.count: " << it.count << endl;
    for (int i = 0; i < it.count; i ++) {
        // px is the color value of pixel
        //Vec3b px = Vec3b(*it);
        //cout << "pos: " << it.pos() << "\t";
        //cout << "bgr: " << px << endl;
        points.push_back(it.pos());
        it++;
    }
    return it.count;
}

void test_iter()
{
    using namespace std;
    using namespace cv;

    Point p1 = Point(50, 5);
    Point p2 = Point(128, 153);
    Mat img = imread(TEST_IMAGE);
    vector<cv::Point> points;

    int ret = iter(img, p1, p2, points);
    if (ret) {
        for (size_t i=0; i<points.size(); i++) {
            cout << points.at(i) << "    ";
        }
        cout << endl;
    } else {
        cout << "there is no points to iterate\n";
    }

}

int main(int argc, char *argv[])
{
    hello_cv2();
    test_iter();
    return 0;
}
