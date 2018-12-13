/**
 **  simple opencv + realsense
 **/

#include "dist.h"
#include "rsutil.h"

#include <iostream>
#include <iomanip>

#define CVUI_IMPLEMENTATION
#include "../../opencv/cvui.h"


auto window_name = "Display Image";

void draw_crosshair(cv::Mat& img, const cv::Point& pt)
{
    using namespace cv;
    const int slen = 40;
    Scalar color = Scalar(0, 0, 0xff);
    circle(img, pt, 8, color);
    Point p1 = Point(pt.x - slen, pt.y );
    Point p2 = Point(pt.x + slen, pt.y );
    line(img, p1, p2, color, 1, LINE_AA);
    Point p3 = Point(pt.x, pt.y - slen);
    Point p4 = Point(pt.x, pt.y + slen);
    line(img, p3, p4, color, 1, LINE_AA);
}


// Neighbors function returns 12 fixed neighboring pixels in an image
//std::array<pixel, 12> neighbors(rs2::depth_frame frame, pixel p);


int main()
{
    const int DEFAULT_WIDTH = 640;
    const int DEFAULT_HEIGHT = 480;

    namedWindow(window_name, WINDOW_AUTOSIZE);
    moveWindow(window_name, 0, 0);
    cvui::init(window_name);

    if ( !rsutil::show_rsinfo() ) {
        printf("no Intel Realsense camera, exit...\n");
        exit(-1);
    }

    //Contruct a pipeline which abstracts the device
    rs2::pipeline pipe;

    //Create a configuration for configuring the pipeline with a non default profile
    rs2::config cfg;

    //Add desired streams to configuration
    //cfg.enable_stream(RS2_STREAM_INFRARED, WIDTH, HEIGHT, RS2_FORMAT_Y8, 30);
    cfg.enable_stream(RS2_STREAM_COLOR, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, DEFAULT_WIDTH, DEFAULT_HEIGHT, RS2_FORMAT_Z16, 30);

    //Instruct pipeline to start streaming with the requested configuration
    pipe.start(cfg);

    // Camera warmup - dropping several first frames to let auto-exposure stabilize
    rs2::frameset frames;
    for (int i = 0; i < 30; i++)
    {
        //Wait for all configured streams to produce a frame
        frames = pipe.wait_for_frames();
    }

    const int thickness = 2;
    const int radius = 6;
    //const int tail = 5;
    //Scalar white = Scalar(255, 255, 255);
    Point pt;
    cv::Vec3f xyz;

    while (true) {

        frames = pipe.wait_for_frames(); // Wait for next set of frames from the camera

        //rs2::frame ir_frame = frames.first(RS2_STREAM_INFRARED);
        rs2::frame color_frame = frames.get_color_frame();
        rs2::frame depth_frame = frames.get_depth_frame();

        // Creating OpenCV matrix from IR image
        //Mat ir(Size(WIDTH, HEIGHT), CV_8UC1, (void*)ir_frame.get_data(), Mat::AUTO_STEP);
        Mat color(Size(DEFAULT_WIDTH, DEFAULT_HEIGHT), CV_8UC3, (void*)color_frame.get_data(), Mat::AUTO_STEP);

        // Apply Histogram Equalization
        //equalizeHist( ir, ir );
        //applyColorMap(ir, ir, COLORMAP_JET);
        int x = cvui::mouse().x % DEFAULT_WIDTH;
        int y = cvui::mouse().y % DEFAULT_HEIGHT;

        cv::Rect rectangle(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
        int status = cvui::iarea(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
        switch (status) {
            case cvui::CLICK:
                pt.x = x;
                pt.y = y;
                query_uv2xyz(depth_frame, pt, xyz);
                break;
            //case cvui::DOWN:
                //cvui::printf(frame, 240, 70, "Mouse is: DOWN");
                //cout << "mouse down " << x << "," << y << endl;
                //break;
            default:
                break;
        }

        //pixel u = {x, y};
        //auto d = dist_3d(depth_frame, u, v);
        draw_crosshair(color, pt);



        cvui::update();

        // Display the image in GUI
        imshow(window_name, color);

        //cout << "press any key to quit...\n";
        if ( waitKey(1) == 0x1B ) {
            break;
        }
    }

    pipe.stop();
    return 0;
}

