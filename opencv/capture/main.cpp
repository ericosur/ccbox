//#include <QCoreApplication>

int demoCapture();
int demoTest();
int test_realsense();

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    //return a.exec();

    //demoCapture();

#ifdef USE_REALSENSE
    test_realsense();
#else
    if ( demoTest() == -1 ) {
        return -1;
    }
#endif
    //a.quit();

    return 0;
}
