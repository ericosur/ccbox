#include "dist.h"

void display_point(float point[3])
{
    cout << "(" << point[0] << "," << point[1] << "," << point[2] << ")\n";
}

void query_uv2xyz(const rs2::depth_frame& frame, const cv::Point& pt, cv::Vec3f& xyz)
{
    float pixel[2]; // From pixel
    float point[3]; // From point (in 3D)

    pixel[0] = pt.x;
    pixel[1] = pt.y;

    auto dist = frame.get_distance(pixel[0], pixel[1]);

    // Deproject from pixel to point in 3D
    rs2_intrinsics intr = frame.get_profile().as<rs2::video_stream_profile>().get_intrinsics(); // Calibration data
    rs2_deproject_pixel_to_point(point, &intr, pixel, dist);
    cout << "point: ";
    display_point(point);

    xyz[0] = point[0];
    xyz[1] = point[1];
    xyz[2] = point[2];
}

float dist_3d(const rs2::depth_frame& frame, pixel u, pixel v)
{
    float upixel[2]; // From pixel
    float upoint[3]; // From point (in 3D)

    float vpixel[2]; // To pixel
    float vpoint[3]; // To point (in 3D)

    // Copy pixels into the arrays (to match rsutil signatures)
    upixel[0] = u.first;
    upixel[1] = u.second;
    vpixel[0] = v.first;
    vpixel[1] = v.second;

    // Query the frame for distance
    // Note: this can be optimized
    // It is not recommended to issue an API call for each pixel
    // (since the compiler can't inline these)
    // However, in this example it is not one of the bottlenecks
    auto udist = frame.get_distance(upixel[0], upixel[1]);
    auto vdist = frame.get_distance(vpixel[0], vpixel[1]);

    // Deproject from pixel to point in 3D
    rs2_intrinsics intr = frame.get_profile().as<rs2::video_stream_profile>().get_intrinsics(); // Calibration data
    rs2_deproject_pixel_to_point(upoint, &intr, upixel, udist);
    cout << "upoint: ";
    display_point(upoint);
    rs2_deproject_pixel_to_point(vpoint, &intr, vpixel, vdist);
    cout << "vpoint: ";
    display_point(vpoint);

    // Calculate euclidean distance between the two points
    return sqrt(pow(upoint[0] - vpoint[0], 2) +
                pow(upoint[1] - vpoint[1], 2) +
                pow(upoint[2] - vpoint[2], 2));
}

float dist_2d(const pixel& a, const pixel& b)
{
    return pow(a.first - b.first, 2) + pow(a.second - b.second, 2);
}
