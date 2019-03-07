#include "common.h"
#include "rsutil.h"

#include <stdio.h>

#ifdef USE_REALSENSE

namespace rsutil {

using namespace std;
using namespace cv;

bool check_depth(float d)
{
    bool ret = false;
    if (ISNAN(d) || d < MIN_DEPTH || d > MAX_DEPTH) {
        ret = false;
    } else {
        ret = true;
    }

    // if (!ret) {
    //     printf("REJECT: check_depth failed while d=%.2f\n", d);
    // }
    return ret;
}

void display_point(float point[3])
{
    printf("(%.2f,%.2f,%.2f)\n", point[0], point[1], point[2]);
}

float get_distance_mm(const rs2::depth_frame& frame, int u, int v)
{
    float d = frame.get_distance(u, v);
    d *= 1000.0;
    return d;
}

bool query_uv2xyz(const rs2::depth_frame& frame, const cv::Point& pt, cv::Vec3f& xyz, bool debug)
{
    if (!frame) {
        printf("no frame\n");
        return false;
    }

    //printf("enter %s...\n", __func__);
    float pixel[2]; // From pixel
    float point[3]; // From point (in 3D)

    pixel[0] = pt.x;
    pixel[1] = pt.y;

    xyz[0] = 0.0;
    xyz[1] = 0.0;
    xyz[2] = 0.0;

    float dist = frame.get_distance(pixel[0], pixel[1]);
    //printf("get_distance: dist: %.2f\n", dist);
    if (dist < MIN_DEPTH_METER) {
        if (g_verbose) {
            printf("dist failed: %.2f\n", dist);
        }
        return false;
    }

    // Deproject from pixel to point in 3D
    rs2_intrinsics intr = frame.get_profile().as<rs2::video_stream_profile>().get_intrinsics(); // Calibration data
    rs2_deproject_pixel_to_point(point, &intr, pixel, dist);
    if (debug) {
        cout << "point: ";
        rsutil::display_point(point);
    }

    xyz[0] = point[0];
    xyz[1] = point[1];
    xyz[2] = point[2];
    return true;
}


float dist_3d_xyz(const cv::Vec3f xyz1, const cv::Vec3f xyz2)
{
    return sqrt(pow(xyz1[0] - xyz2[0], 2) +
                pow(xyz1[1] - xyz2[1], 2) +
                pow(xyz1[2] - xyz2[2], 2));
}


// given pixel u, v, internally transfer to xyz and calculate distance
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
    float udist = get_distance_mm(frame, upixel[0], upixel[1]);
    float vdist = get_distance_mm(frame, vpixel[0], vpixel[1]);

    if (!check_depth(udist)) {
        return 0.0;
    }
    if (!check_depth(vdist)) {
        return 0.0;
    }


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

bool get_vec_deg(cv::Mat& v, double& degree, bool debug)
{
    float d1[] = {1000, 0, 0};
    Mat w(3,1,CV_32F,d1);

    double upper = w.dot(v);
    if (debug) {
        cout << "get_vec_deg: dot = " << upper << endl;
    }

    double k1 = norm(w, NORM_L2);
    double k2 = norm(v, NORM_L2);
    double lower = k1 * k2;
    if (lower != 0.0) {
        double costheta = upper / lower;
        degree = acos(costheta) * 180.0 / M_PI;
        if (debug) {
            cout << "get_vec_deg: deg: " << degree << endl;
        }
        return true;
    }
    return false;
}

bool show_rsinfo()
{
    using namespace std;
    cout << "librealsense SDK version: " << RS2_API_VERSION_STR << endl;

    rs2::context ctx;
    auto devices = ctx.query_devices();
    size_t device_count = devices.size();
    for (size_t i = 0; i < device_count; ++i)
    {
        auto dev = devices[i];
        string dev_name = dev.get_info(RS2_CAMERA_INFO_NAME);
        if ( dev_name.find("Intel") != std::string::npos) {
            //cout << "found Intel!" << '\n';
            if ( dev.get_info(RS2_CAMERA_INFO_NAME) ) {
                cout << left
                << setw(4) << "[" << i << "] "
                << setw(30) << dev.get_info(RS2_CAMERA_INFO_NAME)
                << setw(20) << dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER)
                << setw(20) << dev.get_info(RS2_CAMERA_INFO_FIRMWARE_VERSION)
                << endl;
            }
            return true;
        }
    }

    // no camera name with "Intel"
    return false;
}

float get_depth_scale(rs2::device dev)
{
    // Go over the device's sensors
    for (rs2::sensor& sensor : dev.query_sensors())
    {
        // Check if the sensor if a depth sensor
        if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
        {
            return dpt.get_depth_scale();
        }
    }
    throw std::runtime_error("Device does not have a depth sensor");
}

void remove_background(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist)
{
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];

            // Check if the depth value is invalid (<=0) or greater than the threashold
            if (pixels_distance <= 0.f || pixels_distance > clipping_dist)
            {
                // Calculate the offset in other frame's buffer to current pixel
                auto offset = depth_pixel_index * other_bpp;

                // Set pixel to "background" color (0x999999)
                std::memset(&p_other_frame[offset], 0x99, other_bpp);
            }
        }
    }
}


// must-be-at-end-of-file
}   // namespace rsutil
#endif
