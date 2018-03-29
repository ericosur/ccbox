#include <pbox/pbox.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#ifdef USE_JSON
#include <fstream>
#include <nlohmann/json.hpp>
#endif

#define JSON_FILE    "../setting.json"
#define INPUT_IMAGE  "lena.jpg"
#define OUTPUT_IMAGE "out.png"

using namespace cv;
using namespace std;

string input_img;
string output_img;

#ifdef USE_JSON
bool load_json(const string& json_file,
               string& input_fn,
               string& output_fn)
{
    if (!pbox::is_file_exist(JSON_FILE)) {
        cout << "json not found..." << endl;
        return false;
    }
    nlohmann::json json;
    try {
        ifstream infile(JSON_FILE);

        infile >> json;
        string home = pbox::get_home();
        string datadir = json.at("datadir");
        if (json.at("use_home")) {
            datadir = home + "/" + datadir;
        }
        input_fn = json.at("input");
        input_fn = datadir + '/' + input_fn;
        output_fn = json.at("output");

        cout << "input: " << input_fn << endl;
        cout << "output: " << output_fn << endl;
    } catch (nlohmann::json::parse_error& e) {
        cout << "parse json error: " << e.what();
        return false;
    }
    return true;
}
#endif

bool init()
{
    namedWindow("src", WINDOW_AUTOSIZE);
    namedWindow("dst", WINDOW_AUTOSIZE);
    moveWindow("src", 0, 0);
    moveWindow("dst", 400, 0);
#ifdef USE_JSON
    if ( !load_json(JSON_FILE, input_img, output_img) ) {
        input_img = INPUT_IMAGE;
        output_img = OUTPUT_IMAGE;
    } else {
        // load ok
    }
#else
    input_img = INPUT_IMAGE;
    output_img = OUTPUT_IMAGE;
#endif


    if (pbox::is_file_exist(input_img)) {
        return true;
    } else {
        cout << "file not found: " << input_img << endl;
    }

    return false;
}

Mat rotate(Mat src, double angle)
{
    Mat dst;
    Point2f pt(src.cols/2., src.rows/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, Size(src.cols, src.rows));
    return dst;
}

int main()
{
    if (!init()) {
        cout << "something wrong!\n";
        return 1;
    }

    bool test = true;
    Mat src = imread(input_img);

    imshow("src", src);

    if (test) {
        Mat dst = rotate(src, 45);
        imshow("dst", dst);
        imwrite(output_img, dst);
    }

    waitKey(0);
    destroyAllWindows();

    return 0;
}
