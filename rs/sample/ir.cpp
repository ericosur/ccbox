/**
 **  simple opencv + realsense
 **/

#include "dist.h"

// Neighbors function returns 12 fixed neighboring pixels in an image
//std::array<pixel, 12> neighbors(rs2::depth_frame frame, pixel p);


int main()
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    //Contruct a pipeline which abstracts the device
    rs2::pipeline pipe;

    //Create a configuration for configuring the pipeline with a non default profile
    rs2::config cfg;

    //Add desired streams to configuration
    //cfg.enable_stream(RS2_STREAM_INFRARED, WIDTH, HEIGHT, RS2_FORMAT_Y8, 30);
    cfg.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, 30);
    cfg.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, 30);

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
    Scalar white = Scalar(255, 255, 255);

    while (true) {

        frames = pipe.wait_for_frames(); // Wait for next set of frames from the camera

        //rs2::frame ir_frame = frames.first(RS2_STREAM_INFRARED);
        rs2::frame color_frame = frames.get_color_frame();
        rs2::frame depth_frame = frames.get_depth_frame();

        // Creating OpenCV matrix from IR image
        //Mat ir(Size(WIDTH, HEIGHT), CV_8UC1, (void*)ir_frame.get_data(), Mat::AUTO_STEP);
        Mat color(Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), Mat::AUTO_STEP);

        // Apply Histogram Equalization
        //equalizeHist( ir, ir );
        //applyColorMap(ir, ir, COLORMAP_JET);

        pixel u = {WIDTH/2, HEIGHT/2-100};
        pixel v = {WIDTH/2, HEIGHT/2};
        auto d = dist_3d(depth_frame, u, v);
        cout << "d: " << d << endl;

        circle(color, Point(u.first, u.second), radius, white, thickness);
        circle(color, Point(v.first, v.second), radius, white, thickness);

        // Display the image in GUI
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", color);

        //cout << "press any key to quit...\n";
        if ( waitKey(1) == 0x1B ) {
            break;
        }
    }

    pipe.stop();
    return 0;
}

