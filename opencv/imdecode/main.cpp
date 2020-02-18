#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#ifdef USE_MYTOOL
#include <mytool/mytool.h>
#endif

//this program is used for testing opencv encode and decode for jgeg pictures

/**
 ** refer to: https://stackoverflow.com/questions/259297/how-do-you-copy-the-contents-of-an-array-to-a-stdvector-in-c-without-looping
 **
 **/

typedef unsigned char byte;

int main()
{
    using namespace std;
    using namespace cv;

    const auto fn = "../../data/lena600.jpg";
    const auto winame = "opencv";

    cout << "read image from: " << fn << endl;
    Mat tstMat = imread(fn);

    namedWindow(winame);
    moveWindow(winame, 0, 0);

    vector<byte> inImage;
    imencode(".jpg", tstMat, inImage);
    size_t datalen = inImage.size();
    cout << "datalen: " << datalen << "\n";

    byte* buffer = (byte*)malloc(inImage.size());
    std::copy(inImage.begin(), inImage.end(), buffer);
#ifdef USE_MYTOOL
    const auto header_size = 128;
    mytool::dump((const char*)buffer, header_size);
#endif
    vector<unsigned char> buffVec;
    buffVec.insert(buffVec.end(), &buffer[0], &buffer[datalen]);
    Mat show = imdecode(buffVec, IMREAD_COLOR);

    imshow(winame, show);
    cv::waitKey(0);
    free(buffer);

    return 0;
}
