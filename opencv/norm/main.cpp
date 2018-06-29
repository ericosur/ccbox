#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;

void print_v(const vector<double>& v)
{
    for (auto& x: v) {
        cout << x << ", ";
    }
    cout << endl;
}

/*
 refer to https://docs.opencv.org/2.4/modules/core/doc/operations_on_arrays.html#norm
 */
void test()
{
    using namespace cv;

    vector<double> v = {6.0, -1.0, 2.0};

    cout << "show v: ";
    print_v(v);
    cout << "NORM_L1: " << norm(v, NORM_L1) << endl;
    cout << "NORM_L2: " << norm(v, NORM_L2) << endl;
    cout << "NORM_INF: " << norm(v, NORM_INF) << endl;
}


int main()
{
    test();
    return 0;
}