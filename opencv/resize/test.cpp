#include "mytool/mytool.h"

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#define SETTING_JSON        "../setting.json"
#define DEFAULT_TESTIMAGE   "lena.png"
#define REPEAT      200

void test()
{
    using namespace std;
    using namespace cv;

    string test_image = mytool::get_string_from_jsonfile(SETTING_JSON, "test_image", DEFAULT_TESTIMAGE);
    if ( !mytool::is_file_exist(test_image) ) {
        cout << "test iamge not found: " << test_image << "\n";
        return;
    }

    Mat src = imread(test_image);
    Mat dst, dst2, dst3;

    for (int i = REPEAT; i > 0; --i) {
        // specify fx and fy and let the function compute the destination image size.
        resize(src, dst, Size(), 0.01*i, 0.01*i);
        resize(src, dst2, Size(), 0.01*i, 0.01*i, INTER_AREA);
        resize(src, dst3, Size(), 0.01*i, 0.02*i, INTER_CUBIC);
    }

    // imshow("test", dst);
    // waitKey(0);
    // destroyAllWindows();
}
