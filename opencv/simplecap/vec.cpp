
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <math.h>

using namespace std;
using namespace cv;

void test();
void test2();
void test3();
void test_gemm();

void testdot()
{
    float d1[] = {1, 0, 1};
    Mat w(3,1,CV_32F,d1);
    float d2[] = {-0.5, 0, 2};
    Mat v(3,1,CV_32F,d2);

    double upper = w.dot(v);
    cout << "dot = " << upper << endl;

    double k1 = norm(w, NORM_L2);
    double k2 = norm(v, NORM_L2);
    double lower = k1 * k2;
    if (lower != 0.0) {
        double costheta = upper / lower;
        double degree = acos(costheta) * 180.0 / M_PI;
        cout << "degree: " << degree << endl;
    }

}

void testxyz()
{
    float data[] = {1, -1, 0, 0, 1, 1};
    Mat A(3,2,CV_32FC1, data);
    float data2[] = {-0.5, 3, 2};
    Mat v(3,1,CV_32FC1, data2);

    cout << "A = " << A
        << endl;
    cout << "v = " << v << endl;
    Mat A_t = A.t();
    cout << "A.t() = " << A_t << endl;


    Mat Ata;
    mulTransposed(A, Ata, true);
    cout << "A t A = " << Ata << endl;

    // Mat aat;
    // mulTransposed(A, aat, false);
    // cout << "A A t =" << aat << endl;

    Mat inv;
    invert(Ata, inv);
    cout << "inv = " << inv << endl;

    Mat ans;
    ans = A * inv * A_t * v;
    cout << "ans = " << ans << endl;
}

void tii()
{
    float data[] = {10, 0, 0, 3};
    Mat B(2, 2, CV_32F, data);
    Mat ans;
    invert(B, ans);
    cout << "ans: " << ans << endl;

}

void iiimmiii()
{
    float d[] = {4, 7, 2, 6};
    Mat m(2,2,CV_32F, d);
    cout << m << endl;
    Mat r;
    invert(m, r);
    cout << r << endl;
    cout << (m * r) << endl;
}

int main()
{
    //testxyz();
    testdot();
    //tii();
    //iiimmiii();
    //test();
    //test2();
    //test3();
    //test_gemm();

    return 0;
}

void test()
{
    float data[] = {1,1,2,-1,-2,0,1,1};
    Mat A(4,2,CV_32FC1, data);
    float data2[] = {1,2,3,4};
    Mat v(4,1,CV_32FC1, data2);

    cout << "A = " << A
        << "rows " << A.rows
        << "cols " << A.cols
        << endl;
    cout << "v = " << v << endl;
    Mat A_t = A.t();
    cout << "A.t() = " << A_t << endl;

    // Mat A1 = A + Mat::eye(A.size(), A.type())*lambda;
    // Mat C = A1.t()*A1; // compute (A + lambda*I)^t * (A + lamda*I)

    Mat Ata;
    mulTransposed(A, Ata, true);
    cout << "A t A = " << Ata << endl;

    Mat aat;
    mulTransposed(A, aat, false);
    cout << "A A t =" << aat << endl;
#if 0
    Mat tmp;
    tmp = A_t * A;
    cout << "tmp = " << tmp << endl;
    Mat t2;
    t2 = A * A_t;
    cout << "tmp = " << t2 << endl;
#endif

    Mat inv;
    invert(Ata, inv);
    cout << "inv = " << inv << endl;

    Mat ans;
    ans = A * inv * A_t * v;
    cout << "ans = " << ans << endl;
}

void test2()
{
    float data[] = {3,1,2,0,1,5};
    Mat a(2,3,CV_32FC1,data);
    cout << a << endl;
    float d2[] = {4,3,1,1,6,0};
    Mat b(3,2,CV_32FC1,d2);
    cout << b << endl;
    float d3[] = {-10,-2,-20,10};
    Mat c(2,2,CV_32FC1,d3);
    Mat dst;
    //cvMatMulAdd(a, b, c, dst);
    dst = a * b + c;
    cout << "dst = " << dst << endl;
}

void test3()
{
    float data[] = {3,1,2,0,1,5};
    Mat a(2,3,CV_32FC1,data);
    Mat b = Mat::eye(3, 3, CV_32F);
    Mat p = a * b;
    cout << p << endl;
    Mat z = Mat::zeros(3, 3, CV_32F);
    Mat q = a * z;
    cout << q << endl;
    Mat won = Mat::ones(3, 3, CV_32F);
    Mat r = a * won;
    cout << r << endl;
}

void test_gemm()
{
    float data[] = {3,1,2,0,1,5};
    Mat a(2,3,CV_32FC1,data);
    float d2[] = {1,0,0,0,1,0,0,0,1};
    Mat b(3,3,CV_32FC1,d2);
    Mat c = Mat::zeros(2,3,CV_32FC1);

    // R = a * b + c
    Mat R;
    gemm(a, b, 1.0, c, 1.0, R, 0);
    cout << "R = " << R << endl;

    Mat r;
    r = a * b;
    R = r + c;
    cout << "R = " << R << endl;
}
