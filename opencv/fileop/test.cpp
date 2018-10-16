#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/persistence.hpp>

#define SETTING_JSON    "../setting.json"

void test()
{
    using namespace std;
    using namespace cv;

    cout << "read values from json file...\n";
    FileStorage fs(SETTING_JSON, FileStorage::READ);
    string name = fs["name"];
    string date = fs["date"];
    cout << "name: " << name << endl
        << "date: " << date << endl;
    fs.release();
}
