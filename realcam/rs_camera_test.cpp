#include "rs_camera_test.hpp"

using namespace std;

#define WIDTH 640
#define HEIGHT 480
#define FPS 30

static volatile int Camera_SIGINT = 0;
static void Camera_SIGINT_handler(int sig)
{
	Camera_SIGINT = 1;
}

static void rgp_save(char *FileName, rs2::video_frame& other_frame){
	uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

	FILE *fp1 = fopen(FileName, "wb");
	fwrite(p_other_frame, 1, (sizeof(uint8_t) * WIDTH * HEIGHT * 3), fp1);
	fclose(fp1);

//	uint8_t* buffer = (uint8_t* )malloc(sizeof(uint8_t) * WIDTH * HEIGHT * 3);
//	FILE *fp2 = fopen(FileName, "rb");
//	fread(buffer, (sizeof(uint8_t) * WIDTH * HEIGHT * 3), 1, fp2);
//	fclose(fp2)
//
//	char bin_filename[128];
//	sprintf(bin_filename, "%s.jpg", FileName);
//	FILE *fp3 = fopen(bin_filename, "wb");
//	jpeg(fp3, buffer, WIDTH, HEIGHT, 100);
//	fclose(fp3);
//
//	free(buffer);
//;
}

static void depth_save(char *rgb_FileName, char *FileName, rs2::depth_frame& depth_frame, float depth_scale){
	uint16_t* p_depth_frame = reinterpret_cast<uint16_t*>(const_cast<void*>(depth_frame.get_data()));

//	float *depth_distance_buffer = (float *)malloc(sizeof(float) * WIDTH * HEIGHT);
	for (int i=0; i< (WIDTH * HEIGHT); i++)
		p_depth_frame[i] = p_depth_frame[i] * depth_scale * 100; //cm unite

	FILE *fp1 = fopen(FileName, "wb");
	fwrite(p_depth_frame, 1, (sizeof(uint16_t) * WIDTH * HEIGHT), fp1);
	fclose(fp1);

	uint8_t* rgb_buffer = (uint8_t* )malloc(sizeof(uint8_t) * WIDTH * HEIGHT * 3);
	FILE *fp2 = fopen(rgb_FileName, "rb");
	fread(rgb_buffer, (sizeof(uint8_t) * WIDTH * HEIGHT * 3), 1, fp2);
	fclose(fp2);

	uint16_t* depth_buffer = (uint16_t* )malloc(sizeof(uint16_t) * WIDTH * HEIGHT);
	FILE *fp3 = fopen(FileName, "rb");
	fread(depth_buffer, (sizeof(uint16_t) * WIDTH * HEIGHT), 1, fp3);
	fclose(fp3);

	#pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < HEIGHT; y++)
    {
        auto depth_pixel_index = y * WIDTH;
        for (int x = 0; x < WIDTH; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = (uint16_t)depth_buffer[depth_pixel_index];

            // Check if the depth value is invalid (<=0) or greater than the threashold
            if (pixels_distance <= 0 || pixels_distance > 3 * 100)
            {
                // Calculate the offset in other frame's buffer to current pixel
                auto offset = depth_pixel_index * 3;

                // Set pixel to "background" color (0x999999)
                std::memset(&rgb_buffer[offset], 0x99, 3);
            }
        }
    }

	char jpg_filename[128];
	sprintf(jpg_filename, "%s.jpg", FileName);
	FILE *fp4 = fopen(jpg_filename, "wb");
	//jpeg(fp4, rgb_buffer, WIDTH, HEIGHT, 100);
	fclose(fp4);

	free(rgb_buffer);
	free(depth_buffer);

//	free(depth_distance_buffer);
}

int main (int argc, char *argv[])
{
	rs2::colorizer color_map;                          // Helper to colorize depth images

	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_DEPTH, 0, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);
	cfg.enable_stream(RS2_STREAM_COLOR, 0, WIDTH, HEIGHT, RS2_FORMAT_RGB8, FPS);

	// Create a pipeline to easily configure and start the camera
    rs2::pipeline pipe;
    //Calling pipeline's start() without any additional parameters will start the first device
    // with its default streams.
    //The start function returns the pipeline profile which the pipeline used to start the device
    rs2::pipeline_profile profile = pipe.start(cfg);
	//rs2::pipeline_profile profile = pipe.start();

	// Each depth camera might have different units for depth pixels, so we get it here
    // Using the pipeline's profile, we can retrieve the device that the pipeline uses
    float depth_scale = get_depth_scale(profile.get_device());

    //Pipeline could choose a device that does not have a color stream
    //If there is no color stream, choose to align depth to another stream
    rs2_stream align_to = find_stream_to_align(profile.get_streams());

    // Create a rs2::align object.
    // rs2::align allows us to perform alignment of depth frames to others frames
    //The "align_to" is the stream type to which we plan to align depth frames.
    rs2::align align(align_to);

    // Define a variable for controlling the distance to clip
    float depth_clipping_distance = 3.f;

//	std::vector<rs2::stream_profile> stream_profiles = profile.get_streams();
//	for (int i = 0; i < stream_profiles.size(); i++) {
//		cout << stream_profiles[i].stream_name() << endl;
//		printf("[%d] fps : %d format %d\n, stream_index %d stream_type %d\n",
//			   i, stream_profiles[i].fps(), stream_profiles[i].format(), stream_profiles[i].stream_index(),
//			   stream_profiles[i].stream_type());
//	}

	signal(SIGINT, Camera_SIGINT_handler);

	char rgb_filename[32];
	char depth_filename[32];
	for (int i = 0 ; ; i = ( i + 1 ) % 100) {
		if (Camera_SIGINT) break;
		printf("%d\n", i);

		// Using the align object, we block the application until a frameset is available
		rs2::frameset frameset = pipe.wait_for_frames();

		// rs2::pipeline::wait_for_frames() can replace the device it uses in case of device error or disconnection.
		// Since rs2::align is aligning depth to some other stream, we need to make sure that the stream was not changed
		//  after the call to wait_for_frames();
		if (profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams())) {
			//If the profile was changed, update the align object, and also get the new device's depth scale
			profile = pipe.get_active_profile();
			align_to = find_stream_to_align(profile.get_streams());
			align = rs2::align(align_to);
			depth_scale = get_depth_scale(profile.get_device());
		}

		//Get processed aligned frame
		auto processed = align.process(frameset);

		// Trying to get both other and aligned depth frames
		rs2::video_frame other_frame = processed.first(align_to);
		rs2::depth_frame aligned_depth_frame = processed.get_depth_frame();

		// Passing both frames to remove_background so it will "strip" the background
        // NOTE: in this example, we alter the buffer of the other frame, instead of copying it and altering the copy
        //       This behavior is not recommended in real application since the other frame could be used elsewhere
//		remove_background(other_frame, aligned_depth_frame, depth_scale, depth_clipping_distance);
//		sprintf(rgb_filename, "/tmp/rgb%d.jpg", i);
//		sprintf(depth_filename, "/tmp/depth%d.jpg", i);
//		unsigned char* rgb1 = (unsigned char*)other_frame.get_data();
//		FILE* out1 = fopen(rgb_filename, "wb");
//		jpeg(out1, rgb1, other_frame.get_width(), other_frame.get_height(), 100);
//		fclose(out1);
//
//		unsigned char* rgb2 = (unsigned char*)color_map(aligned_depth_frame).get_data();
//		FILE* out2 = fopen(depth_filename, "wb");
//		jpeg(out2, rgb2, other_frame.get_width(), other_frame.get_height(), 100);
//		fclose(out2);

		sprintf(rgb_filename, "/tmp/rgb%d.bin", i);
		sprintf(depth_filename, "/tmp/depth%d.bin", i);
		rgp_save(rgb_filename, other_frame);
		depth_save(rgb_filename, depth_filename, aligned_depth_frame, depth_scale);
	}

	pipe.stop();

	return(0);
}
