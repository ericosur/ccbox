#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

#include <fstream>
#include <nlohmann/json.hpp>
const auto json_file = "../fe2d.json";
const auto window_name = "feature 2D";

int Threshold = 50;
std::vector<std::string> files;

bool load_json()
{
    using namespace std;
    using namespace nlohmann;

    cout << "load settings from: " << json_file << "\n";
    json j;
    json jarray = json::array();

    try {
        ifstream infile(json_file);
        infile >> j;
        jarray = j.at("files");
        Threshold = j.at("threshold");
        //cout << setw(4) << j << endl;
    }
    catch (json::parse_error& e) {
        cout << "parse json error: " << e.what();
        return false;
    }

    // maybe there is a more efficient way to load json array into std::vector
    for (json::iterator it = jarray.begin(); it != jarray.end(); ++it) {
        //cout << *it << endl;
        files.push_back(it->get<string>());
    }

    return true;
}

void load_images(std::vector<cv::Mat>& imgs, std::vector<std::string>& files)
{
    using namespace std;

    for (vector<string>::iterator it = files.begin(); it != files.end(); ++it) {
        //cout << *it << endl;
        imgs.push_back(cv:: imread(*it));
    }
}

int test()
{
    using namespace std;
    using namespace cv;

    const auto RADIUS = 5;
    vector<Mat> imgs;

    load_json();    // will fill vector 'files'
    load_images(imgs, files);   // fill 'imgs'

    // vector of keyPoints
    vector<vector<KeyPoint>> keypoints;

    // construction of the fast feature detector object
    cout << "threshold = " << Threshold << endl;
    Ptr<FeatureDetector> detector = FastFeatureDetector::create(Threshold);

    detector->detect(imgs, keypoints);

    size_t kp0size = 0;
    size_t kp1size = 0;
    if (keypoints.size() >= 2) {
        kp0size = keypoints[0].size();
        kp1size = keypoints[1].size();
    }
    if (kp0size > kp1size) {
        cout << "[WARNING]: kp0size > kp1size!!!" << endl;
    }
    for (size_t ii = 0; ii < keypoints.size(); ii++) {
        const vector<KeyPoint>& kp = keypoints[ii];
        cout << "ii:" << ii << ", " << "sz: " << kp.size() << endl;

        for (size_t jj = 0; jj < kp0size; jj++) {
            const KeyPoint& pt = kp[jj];
            circle(imgs[ii], Point(pt.pt.x, pt.pt.y), RADIUS, Scalar(255,255,255,255));
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

    Mat combined;

    namedWindow(window_name);
    moveWindow(window_name, 0, 0);
    hconcat(imgs[0], imgs[1], combined);
    imshow(window_name, combined);
    cvWaitKey(0);
    destroyAllWindows();

    return 0;
}

int main()
{
    test();
    return 0;
}
