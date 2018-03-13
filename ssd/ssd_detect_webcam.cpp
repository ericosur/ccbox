// This is a demo code for using a SSD model to do detection.
// The code is modified from examples/cpp_classification/classification.cpp.
// Usage:
//    ssd_detect [FLAGS] model_file weights_file list_file
//
// where model_file is the .prototxt file defining the network architecture, and
// weights_file is the .caffemodel file containing the network parameters, and
// list_file contains a list of image files with the format as follows:
//    folder/img1.JPEG
//    folder/img2.JPEG
// list_file can also contain a list of video files with the format as follows:
//    folder/video1.mp4
//    folder/video2.mp4
//
#include <caffe/caffe.hpp>

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif  // USE_OPENCV

#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <pbox/pbox.h>
#include <pbox/MY_IPC.hpp>

#ifdef USE_OPENCV
using namespace caffe;  // NOLINT(build/namespaces)

//#define USE_DISTWIN
#ifdef USE_DISTWIN
#define DIST_WINDOW   "dist"
#endif

#define NO_REALSENSE
#define RASMUS_HACK

#ifdef RASMUS_HACK

#ifdef USE_OPENCV_CAPTURE
#error should not use opencv VideoCapture
#endif

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_SEND_SIZE           512
#define MESSAGE_TYPE       9
#define MESGQKEY        0x6789CAFE

#define MAX_BUFFER_SIZE   (640*480*3)
#define SHMKEY  0x0111C0DE

#define OUTPUT_SSD_JSON    "/tmp/ssd.json"

typedef uint8_t byte;

struct mymsgbuf {
    long mtype;
    char mtext[MAX_SEND_SIZE];
};

struct shm_st {
    uint32_t size;
    byte buffer[MAX_BUFFER_SIZE];
};

std::string wait_msgq()
{
    key_t key = MESGQKEY;
    int msgqueue_id;
    struct mymsgbuf qbuf;
    std::string result;
    bool debug_msg = false;

    //key = ftok(MSGQ_FILE, 'm');
    int type = MESSAGE_TYPE;
    if((msgqueue_id = msgget(key, IPC_CREAT|0660)) == -1) {
        perror("msgsnd: msgget");
        return "error"; // error here
    }
    if (debug_msg)
      printf("msgq key:0x%08x\n", msgqueue_id);
    while ( true ) {
        qbuf.mtype = MESSAGE_TYPE;
        int ret = msgrcv(msgqueue_id, (struct mymsgbuf *)&qbuf, MAX_SEND_SIZE, type, 0);
        if (ret > 0) {
          printf("recv text: %s\n", qbuf.mtext);
          result = qbuf.mtext;
          break;
        }
        usleep(250*1000);
    }
    return result;
}

void remove_msgq()
{
    int msg_id = msgget(MESGQKEY, IPC_EXCL|0660);
    if(msg_id == -1) {
        //perror("msgget");
        // and exit...
    } else if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        // and exit...
    } else {
      std::cerr << "msgq used by ssd is removed\n";
    }
}

void remove_shm()
{
    printf("%s\n", __func__);
    int shm_id;
    // get id of shm
    shm_id = shmget((key_t)SHMKEY, sizeof(shm_st), 0600 | IPC_EXCL);
    if (shm_id == -1) {
        perror("shmget");
        return;
    }
    // destroy shared memory
    if (shmctl(shm_id, IPC_RMID, 0) == -1) {
        perror("shmctl");
        return;
    }
}

void my_handle_ctrlc(int s)
{
  printf("caught signal %d\n", s);
  remove_msgq();
  exit(1);
}

bool read_from_shm(uint32_t& size, void* buffer)
{
    int shm_id;
    shm_st *st;
    // get the ID of shared memory,
    // fail if specified shmem not created
    shm_id = shmget((key_t)SHMKEY, sizeof(shm_st), 0600 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget");
        return false;
    }
    // attach shared memory
    st = (shm_st*)shmat(shm_id, (void *)0, 0);
    if (st == (void *)-1) {
        perror("shmget");
        return false;
    }
    size = st->size;
    memcpy(buffer, st->buffer, size);
    // detach shared memory
    if (shmdt(st) == -1) {
        perror("shmdt");
        return false;
    }
    return true;
}

#endif // rasmus_hack

class SsdSetting
{
public:
  bool fill_values(int argc, char** argv) {
    if (argc >= 7) {
      model_file = argv[1];
      weights_file = argv[2];
      file_type = argv[3];
      confidence_threshold = atof(argv[4]);
      label_id = atoi(argv[5]);
      label_name = argv[6];
      return true;
    } else {
      fprintf(stderr, "%s\n", "insufficient number of arguments...");
      return false;
    }
  }

  bool read_values_from_json(const std::string& json_file) {
    if ( pbox::is_file_exist(json_file) ) {
      model_file = pbox::get_string_from_jsonfile(json_file, "model_file");
      weights_file = pbox::get_string_from_jsonfile(json_file, "weights_file");
      file_type = pbox::get_string_from_jsonfile(json_file, "file_type");
      confidence_threshold = pbox::get_double_from_jsonfile(json_file, "confidence_threshold");
      label_id = pbox::get_int_from_jsonfile(json_file, "label_id");
      label_name = pbox::get_string_from_jsonfile(json_file, "label_name");
      direct_use_realsense = pbox::get_int_from_jsonfile(json_file, "direct_use_realsense");
      wait_msgq = pbox::get_int_from_jsonfile(json_file, "wait_msgq");
      wait_myipc = pbox::get_int_from_jsonfile(json_file, "wait_myipc");
      wait_myipc_tag = pbox::get_string_from_jsonfile(json_file, "wait_myipc_tag");
      do_imshow = pbox::get_int_from_jsonfile(json_file, "do_imshow");
      show_fps = pbox::get_int_from_jsonfile(json_file, "show_fps");
      testraw = pbox::get_int_from_jsonfile(json_file, "testraw");
      rawbin_dir = pbox::get_string_from_jsonfile(json_file, "rawbin_dir");
      return true;
    } else {
      fprintf(stderr, "%s\n", "specified json not found...");
      return false;
    }
  }

public:
  string model_file;
  string weights_file;
  string file_type = "realsense";
  float confidence_threshold = 0.33;
  int label_id = 15;
  string label_name = "person";
  int direct_use_realsense = 0;
  int wait_msgq = 1;
  int wait_myipc = 0;
  string wait_myipc_tag = "photo_index";
  int do_imshow = 0;
  int show_fps = 1;
  int testraw = 1;
  string rawbin_dir = "/tmp";
};


class Detector {
 public:
  Detector(const string& model_file,
           const string& weights_file,
           const string& mean_file,
           const string& mean_value);

  std::vector<vector<float> > Detect(const cv::Mat& img);

 private:
  void SetMean(const string& mean_file, const string& mean_value);

  void WrapInputLayer(std::vector<cv::Mat>* input_channels);

  void Preprocess(const cv::Mat& img,
                  std::vector<cv::Mat>* input_channels);

 private:
  shared_ptr<Net<float> > net_;
  cv::Size input_geometry_;
  int num_channels_;
  cv::Mat mean_;
};

Detector::Detector(const string& model_file,
                   const string& weights_file,
                   const string& mean_file,
                   const string& mean_value)
{
#ifdef CPU_ONLY
  Caffe::set_mode(Caffe::CPU);
#else
  Caffe::set_mode(Caffe::GPU);
#endif
  /* Load the network. */
  net_.reset(new Net<float>(model_file, TEST));
  net_->CopyTrainedLayersFrom(weights_file);

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  Blob<float>* input_layer = net_->input_blobs()[0];
  num_channels_ = input_layer->channels();
  CHECK(num_channels_ == 3 || num_channels_ == 1)
    << "Input layer should have 1 or 3 channels.";
  input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

  /* Load the binaryproto mean file. */
  SetMean(mean_file, mean_value);
}

std::vector<vector<float> > Detector::Detect(const cv::Mat& img)
{
  Blob<float>* input_layer = net_->input_blobs()[0];
  input_layer->Reshape(1, num_channels_,
                       input_geometry_.height, input_geometry_.width);
  /* Forward dimension change to all layers. */
  net_->Reshape();

  std::vector<cv::Mat> input_channels;
  WrapInputLayer(&input_channels);

  Preprocess(img, &input_channels);

  net_->Forward();

  /* Copy the output layer to a std::vector */
  Blob<float>* result_blob = net_->output_blobs()[0];
  const float* result = result_blob->cpu_data();
  const int num_det = result_blob->height();
  vector<vector<float> > detections;
  for (int k = 0; k < num_det; ++k) {
    if (result[0] == -1) {
      // Skip invalid detection.
      result += 7;
      continue;
    }
    vector<float> detection(result, result + 7);
    detections.push_back(detection);
    result += 7;
  }
  return detections;
}

/* Load the mean file in binaryproto format. */
void Detector::SetMean(const string& mean_file, const string& mean_value) {
  cv::Scalar channel_mean;
  if (!mean_file.empty()) {
    CHECK(mean_value.empty()) <<
      "Cannot specify mean_file and mean_value at the same time";
    BlobProto blob_proto;
    ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

    /* Convert from BlobProto to Blob<float> */
    Blob<float> mean_blob;
    mean_blob.FromProto(blob_proto);
    CHECK_EQ(mean_blob.channels(), num_channels_)
      << "Number of channels of mean file doesn't match input layer.";

    /* The format of the mean file is planar 32-bit float BGR or grayscale. */
    std::vector<cv::Mat> channels;
    float* data = mean_blob.mutable_cpu_data();
    for (int i = 0; i < num_channels_; ++i) {
      /* Extract an individual channel. */
      cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
      channels.push_back(channel);
      data += mean_blob.height() * mean_blob.width();
    }

    /* Merge the separate channels into a single image. */
    cv::Mat mean;
    cv::merge(channels, mean);

    /* Compute the global mean pixel value and create a mean image
     * filled with this value. */
    channel_mean = cv::mean(mean);
    mean_ = cv::Mat(input_geometry_, mean.type(), channel_mean);
  }
  if (!mean_value.empty()) {
    CHECK(mean_file.empty()) <<
      "Cannot specify mean_file and mean_value at the same time";
    stringstream ss(mean_value);
    vector<float> values;
    string item;
    while (getline(ss, item, ',')) {
      float value = std::atof(item.c_str());
      values.push_back(value);
    }
    CHECK(values.size() == 1 || values.size() == (size_t)num_channels_) <<
      "Specify either 1 mean_value or as many as channels: " << num_channels_;

    std::vector<cv::Mat> channels;
    for (int i = 0; i < num_channels_; ++i) {
      /* Extract an individual channel. */
      cv::Mat channel(input_geometry_.height, input_geometry_.width, CV_32FC1,
          cv::Scalar(values[i]));
      channels.push_back(channel);
    }
    cv::merge(channels, mean_);
  }
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void Detector::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
  Blob<float>* input_layer = net_->input_blobs()[0];

  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

void Detector::Preprocess(const cv::Mat& img,
                            std::vector<cv::Mat>* input_channels) {
  /* Convert the input image to the input image format of the network. */
  cv::Mat sample;
  if (img.channels() == 3 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
  else if (img.channels() == 4 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
  else if (img.channels() == 4 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
  else if (img.channels() == 1 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
  else
    sample = img;

  cv::Mat sample_resized;
  if (sample.size() != input_geometry_)
    cv::resize(sample, sample_resized, input_geometry_);
  else
    sample_resized = sample;

  cv::Mat sample_float;
  if (num_channels_ == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
    sample_resized.convertTo(sample_float, CV_32FC1);

  cv::Mat sample_normalized;
  cv::subtract(sample_float, mean_, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == net_->input_blobs()[0]->cpu_data())
    << "Input channels are not wrapping the input layer of the network.";
}

DEFINE_string(mean_file, "",
    "The mean file used to subtract from the input image.");
DEFINE_string(mean_value, "104,117,123",
    "If specified, can be one value or can be same as image channels"
    " - would subtract from the corresponding channel). Separated by ','."
    "Either mean_file or mean_value should be provided, not both.");
DEFINE_string(file_type, "webcam",
    "The file type in the list_file. Currently support image and video.");
DEFINE_string(out_file, "result.txt",
    "If provided, store the detection results in the out_file.");
DEFINE_double(confidence_threshold, 0.3,
    "Only store detections with score higher than the threshold.");
DEFINE_int32(device_id, 2,
    "The input webcam camera default device is 0");

// rasmus hack, global variables
SsdSetting settings;
const int BUFFER_SIZE = 100;

int fontface = cv::FONT_HERSHEY_SIMPLEX;
double scale = 0.75;
int thickness = 2;
int baseline = 0;

void show_dist(float dist)
{
#ifdef USE_DISTWIN
  // will show dist into another window
  using namespace cv;
  Mat img = Mat::zeros(320, 240, CV_8UC3 );

  Scalar c = Scalar(0, 255, 0);
  double sz = 1.25;
  char str[40];

  if (dist <= -1.0) {
    snprintf(str, 40, "no obj");
  } else {
    snprintf(str, 40, "%.2fm", dist);
  }
  putText(img, str, Point(10, 100), fontface, sz, c);
  imshow(DIST_WINDOW, img);
#else
  // just print out into log
  pbox::mylog("ssd", "show_dist: %.2f", dist);
#endif
}

bool show_fps(cv::Mat& cv_img, double elapsed_time)
{
    static double total_time = 0.0;
    static int frame_count = 0;
    static float fps = 0.0;
    char buffer[BUFFER_SIZE];
    const int max_frame_count = 30;

    //show fps
    if (frame_count == max_frame_count) {
        double avg_time = total_time / frame_count;
        printf( "Detection avg. time is %0.2lf ms for %d frames\n", avg_time, frame_count);
        fps = 1000.0 / avg_time;
        frame_count = 0;
        total_time = 0.0;
    } else {
        total_time += elapsed_time;
        //printf( "Detection time is %0.2lf ms\n", elapsed_time);
        frame_count ++;
    }
    snprintf(buffer, BUFFER_SIZE, "FPS: %.2f", fps);
    cv::Size text = cv::getTextSize(buffer, fontface, scale, thickness, &baseline);
    cv::rectangle(cv_img, cv::Point(0, 0),cv::Point(text.width, text.height + baseline),
          CV_RGB(255, 255, 255), CV_FILLED);
    cv::putText(cv_img, buffer, cv::Point(0, text.height + baseline / 2.),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);

    if (settings.do_imshow) {
      cv::imshow("detections", cv_img);
    } else {
      printf("no show\n");
    }

    // int key = cv::waitKey(1);
    // if ( key == 27 || key == 'q' ) {
    //   return false;
    // } else {
    //   return true;
    // }
    return true;
}


#ifndef NO_REALSENSE
float query_dist_from_detection_box(const std::vector< std::vector<float> >& detections,
                                    int img_cols, int img_rows)
{
  float tmp = 1000.0;
  for (int i = 0; i < detections.size(); ++i) {
    const vector<float>& d = detections[i];
    // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
    CHECK_EQ(d.size(), 7);
    const int label = int(d[1]);
    const float score = d[2];

    const int box_x1 = static_cast<int>(d[3] * img_cols) ;
    const int box_y1 = static_cast<int>(d[4] * img_rows) ;
    const int box_x2 = static_cast<int>(d[5] * img_cols) ;
    const int box_y2 = static_cast<int>(d[6] * img_rows) ;
    float dist = 0.0;
    if (score >= settings.confidence_threshold && pbox::isInterestedLabel(label)) {
      // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
      int qx = box_x1 + (box_x2 - box_x1) / 2;
      int qy = box_y1 + (box_y2 - box_y1) / 2;
      dist = pbox::get_dist_from_point(qx, qy);
      printf("[%d] dist = %.2fm\n", i, dist);
      if (dist > 0.0) {
        if (dist < tmp) {
          tmp = dist;
        }
      }
    }
  }
  return tmp;
}
#endif  // NO_REALSENSE


std::string show_detection_box(cv::Mat& cv_img,
                        const std::vector< std::vector<float> >& detections,
                        bool query_distance=false)
{
  char buffer[BUFFER_SIZE];
  int detected = 0;
  bool is_dog = false;
  //printf("%s\n", __func__);
  for (size_t i = 0; i < detections.size(); ++i) {
    const vector<float>& d = detections[i];
    // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
    CHECK_EQ(d.size(), 7);
    const int label = int(d[1]);
    const float score = d[2];

    const int box_x1 = static_cast<int>(d[3] * cv_img.cols) ;
    const int box_y1 = static_cast<int>(d[4] * cv_img.rows) ;
    const int box_x2 = static_cast<int>(d[5] * cv_img.cols) ;
    const int box_y2 = static_cast<int>(d[6] * cv_img.rows) ;

    float dist = 0.0;

    if (score >= settings.confidence_threshold) {

      if (pbox::is_dog(label)) {
        is_dog = true;
      }
      if (is_dog || pbox::is_person(label)) {

        detected ++;
        // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
        const cv::Scalar& color = cv::Scalar( 255, 255, 255 );

        if (settings.direct_use_realsense && query_distance) {
          int qx = box_x1 + (box_x2 - box_x1) / 2;
          int qy = box_y1 + (box_y2 - box_y1) / 2;
          if (settings.direct_use_realsense) {
            dist = pbox::get_dist_from_point(qx, qy);
            pbox::draw_aim(cv_img, box_x1, box_y1, (box_x2 - box_x1), (box_y2 - box_y1));
            show_dist(dist);
          }
        }

        //draw box
        cv::Point top_left_pt(box_x1, box_y1);
        cv::Point bottom_right_pt(box_x2, box_y2);
        cv::Point bottom_left_pt(box_x1, box_y2);

        cv::rectangle(cv_img, top_left_pt, bottom_right_pt, color, 4);
        if (query_distance) {
          snprintf(buffer, sizeof(buffer), "%s(%d):%.2f,%.2f", pbox::get_label_name(label).c_str(),
                   label, dist, score);
        } else {
          snprintf(buffer, sizeof(buffer), "%s(%d):%.2f", pbox::get_label_name(label).c_str(),
                   label, score);
        }
        cv::Size text = cv::getTextSize(buffer, fontface, scale, thickness, &baseline);

        cv::rectangle(cv_img, bottom_left_pt + cv::Point(0, 0),
            bottom_left_pt + cv::Point(text.width, -text.height-baseline),
            color, CV_FILLED);
        cv::putText(cv_img, buffer, bottom_left_pt - cv::Point(0, baseline),
            fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
      }
    }
  }

  std::string r;
  if (is_dog) {
    r = "dog";
  } else if (detected) {
    r = std::string("detected: " + std::to_string(detected));
  }
  return r;
}

bool load_bin_to_buffer(const char* fn, uint8_t* buffer, size_t buffer_size)
{
    FILE* fptr = fopen(fn, "rb");
    if (fptr == NULL) {
        return false;
    }
    size_t rs = fread(buffer, sizeof(uint8_t), buffer_size, fptr);
    //printf("rs: %d\n", (int)rs);
    (void)rs;
    fclose(fptr);
    return true;
}

uint16_t get_dpeth_pt(uint16_t* buffer, int x, int y)
{
    uint16_t pt = 0;
    for (int j = 0; j < y; j++) {
        for (int i = 0; i < x; i++) {
            pt = *buffer ++;
        }
    }
    return pt;
}

int main(int argc, char** argv)
{
  signal(SIGINT, my_handle_ctrlc);

  //::google::InitGoogleLogging(argv[0]);
  // Print output to stderr (while still logging)
  //FLAGS_alsologtostderr = 1;

// #ifndef GFLAGS_GFLAGS_H_
//   namespace gflags = google;
// #endif

/*
  gflags::SetUsageMessage("Do detection using SSD mode.\n"
        "Usage:\n"
        "    ssd_detect [FLAGS] model_file weights_file list_file\n");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
*/

  if (argc == 2) {
    pbox::mylog("ssd", "use %s as setting file\n", argv[1]);
    if ( settings.read_values_from_json(argv[1]) ) {
      pbox::mylog("ssd", "read settings finished\n");
    } else {
      pbox::mylog("ssd", "read settings failed\n");
      exit(-1);
    }
  } else if (argc < 7) {
    //gflags::ShowUsageWithFlagsRestrict(argv[0], "examples/ssd/ssd_detect");
    pbox::mylog("ssd", "insufficient number of arguments\n");
    exit(-1);
  } else {
    // will use arguments from cli
    if ( settings.fill_values(argc, argv) ) {
      pbox::mylog("ssd", "%s\n", "settings.fill_values ok");
    } else {
      pbox::mylog("ssd", "%s\n", "settings.fill_values failed");
      exit(-1);
    }
  }


  const string& mean_file = FLAGS_mean_file;
  const string& mean_value = FLAGS_mean_value;
  const string& out_file = FLAGS_out_file;

  pbox::mylog("ssd", "start init Detector...\n");
  // Initialize the network.
  Detector detector(settings.model_file, settings.weights_file, mean_file, mean_value);
  pbox::mylog("ssd", "end init Detector...\n");

  // Set the output mode.
  std::streambuf* buf = std::cout.rdbuf();
  std::ofstream outfile;
  if (!out_file.empty()) {
    outfile.open(out_file.c_str());
    if (outfile.good()) {
      buf = outfile.rdbuf();
    }
  }
  std::ostream out(buf);

    if (settings.file_type == "image") {

      // Process image one by one.
      while (true) {
        cv::Mat cv_img = cv::imread(argv[7], -1);
        CHECK(!cv_img.empty()) << "Unable to decode image " << argv[7];
        cv::TickMeter tm;
        tm.start();
        std::vector<vector<float> > detections = detector.Detect(cv_img);
        tm.stop();
        // draw detected box
        show_detection_box(cv_img, detections, false);
        //show fps
        if ( settings.show_fps && !show_fps(cv_img, tm.getTimeMilli()) ) {
          break;
        }
      }
    }
#ifdef USE_OPENCV_CAPTURE
    else if (settings.file_type == "video") {
      // Process image one by one.
      std::ifstream infile(argv[3]);
      std::string file;
      while (infile >> file) {
        cv::VideoCapture cap(file);
        if (!cap.isOpened()) {
          LOG(FATAL) << "Failed to open video: " << file;
        }
        cv::Mat img;
        int frame_count = 0;
        while (true) {
          bool success = cap.read(img);
          if (!success) {
            LOG(INFO) << "Process " << frame_count << " frames from " << file;
            break;
          }
          CHECK(!img.empty()) << "Error when read frame";
          std::vector<vector<float> > detections = detector.Detect(img);

          /* Print the detection results. */
          for (int i = 0; i < detections.size(); ++i) {
            const vector<float>& d = detections[i];
            // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
            CHECK_EQ(d.size(), 7);
            const float score = d[2];
            if (score >= settings.confidence_threshold) {
              out << file << "_";
              out << std::setfill('0') << std::setw(6) << frame_count << " ";
              out << static_cast<int>(d[1]) << " ";
              out << score << " ";
              out << static_cast<int>(d[3] * img.cols) << " ";
              out << static_cast<int>(d[4] * img.rows) << " ";
              out << static_cast<int>(d[5] * img.cols) << " ";
              out << static_cast<int>(d[6] * img.rows) << std::endl;
            }
          }
          ++frame_count;
        }
        if (cap.isOpened()) {
          cap.release();
        }
      }
    }
#endif  // USE_OPENCV_CAPTURE
    else if (settings.file_type == "realsense") {
      //cv::Mat cv_img = cv::Mat::zeros(640,480,CV_8UC3);
      //remove_shm();
/*
IPC_Put_TAG_INT32("audience_vol", percent)

IPC_Put_TAG_INT32("dog_warning", 0)

*/
      char rgbfn[256];
      char depfn[256];
      std::string cmd;
      bool hasRealsenseOn = false;
      std::string ofn;
      const std::string default_img_fn = "/tmp/rs.png";
      //uint32_t readsize = 0;
      //byte buffer[MAX_BUFFER_SIZE];
      std::vector<std::string> v;
      int img_idx = 0;
      using namespace pbox;
      MY_IPC *ipc = NULL;
      if (settings.wait_myipc) {
        ipc = MY_IPC::MY_IPC_GetInstance();
        if (ipc == NULL) {
          fprintf(stderr, "failed to init MY_IPC\n");
          exit(1);
        }
      }

      while (true) {
        cv::Mat cv_img;

        if (settings.direct_use_realsense) {
          //cmd = "realsense";
        } else {

          if (settings.wait_msgq) { // use msgq for IPC
            cmd = wait_msgq();
            // TODO: make dir configurable
            if (settings.testraw) {
              snprintf(rgbfn, 256, "%s/rgb%s.bin", settings.rawbin_dir.c_str(), cmd.c_str());
              snprintf(depfn, 256, "%s/depth%s.bin", settings.rawbin_dir.c_str(), cmd.c_str());
            } else {
              // will use full file path
              snprintf(rgbfn, 256, "%s", cmd.c_str());
            }

            printf("wait_msgq got: %s\n%s\n", rgbfn, depfn);
            v.clear();
            v.push_back("wait for msgq");

          } else if (settings.wait_myipc) { // use MY IPC as IPC
            cmd = "myipc";
            img_idx = ipc->IPC_Get_TAG_INT32(settings.wait_myipc_tag.c_str(), 0); // will block here
            snprintf(rgbfn, sizeof(rgbfn), "%s/rgb%d.bin", settings.rawbin_dir.c_str(), img_idx);
            snprintf(depfn, sizeof(depfn), "%s/depth%d.bin", settings.rawbin_dir.c_str(), img_idx);
            printf("wait_myipc: got %d\n", img_idx);
          }
          //printf("[%d] IPC cmd: %s\n", __LINE__, cmd.c_str());
          //printf("here rgbfn: %s\ndepfn: %s\n", rgbfn, depfn);
        }

        if (cmd == "quit" || cmd == "exit") {
          printf("exit...\n");
          v.push_back("got quit @" + pbox::get_timestring());
          break;
        } else if (settings.direct_use_realsense) {
          v.clear();
          v.push_back(cmd);

          if (!hasRealsenseOn) {
            printf("turn on realsense and use it...\n");
            v.push_back("turn on realsense @" + pbox::get_timestring());
          }
          pbox::init_realsense();

          hasRealsenseOn = true;
          v.push_back("hasRealsenseOn");

          pbox::get_color_mat_from_realsense(cv_img);
          pbox::output_status(OUTPUT_SSD_JSON, v);
        } else if (pbox::is_file_exist(rgbfn)) {
          //printf("rgbfn: %s\n", rgbfn);
          v.clear();
          v.push_back("read from " + std::string(rgbfn));
          const int max_width = 640;
          const int max_height = 480;
          const int rgb_byte = 3;
          const int buffer_size = max_width*max_height*rgb_byte;
          uint8_t buffer[buffer_size];
          if (settings.wait_myipc || settings.testraw) {
            //printf("myipc or testraw\n");
            load_bin_to_buffer(rgbfn, buffer, buffer_size);
            cv_img = cv::Mat(cv::Size(max_width, max_height), CV_8UC3, (void*)buffer);
            cv::cvtColor(cv_img, cv_img, cv::COLOR_RGB2BGR);
            //cv::imshow("raw", cv_img);
            //cv::namedWindow("raw", WINDOW_AUTOSIZE);
            //cv::waitKey(0);
          } else if (settings.wait_msgq) {
            printf("read image from: %s\n", rgbfn);
            cv_img = cv::imread(cmd);
            //imshow("test", cv_img);
          }

          ofn = cmd + ".json";
          cmd = "";

        } else {
          printf("file not found: %s\n", rgbfn);
          //pbox::output_status(OUTPUT_SSD_JSON, v);
          continue;
        }

        //printf("col: %d, row: %d\n", cv_img.cols, cv_img.rows);
        cv::TickMeter tm;
        tm.start();
        std::vector<vector<float> > detections = detector.Detect(cv_img);
        tm.stop();

        v.push_back("take time: " + std::to_string(tm.getTimeMilli()));

        // show detection result
        std::string r;
        if (settings.do_imshow) {
          r = show_detection_box(cv_img, detections, hasRealsenseOn);
          if (r != "") {
            imshow("result", cv_img);
          }
        } else {
          r = pbox::output_detections(ofn, detections, cv_img.cols, cv_img.rows);
          v.push_back(r);
        }
        if (r == "dog") {
          ipc->IPC_Put_TAG_INT32("dog_warning", 1);
          std::cout << "=====>" << r << "!!!!!!!\n";
        } else {
          //ipc->IPC_Put_TAG_INT32("dog_warning", 0);
        }
        //query_dist_from_detection_box(detections, cv_img.cols, cv_img.rows);

        // show fps
        if ( settings.show_fps && settings.do_imshow) {
          show_fps(cv_img, tm.getTimeMilli());
        }
        int k = cv::waitKey(1);
        if (k == 27 || k == 'q') {
          break;
        }
      } // while-loop

      v.push_back("break at " + pbox::get_timestring());
      pbox::output_status(OUTPUT_SSD_JSON, v);

    }
#ifdef USE_OPENCV_CAPTURE
    else if (settings.file_type == "webcam") {
      cv::VideoCapture cap;
      if (!cap.open(atof(argv[7]))) {
        LOG(FATAL) << "Failed to open webcam: " << argv[7];
        CV_Assert("Cam open failed");
      }
      cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
      cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
      cap.set(CV_CAP_PROP_AUTOFOCUS, 0);
      cap.set(CV_CAP_PROP_FPS, 30);
      for(;;) {
        cv::Mat cv_img;
        cap >> cv_img;
        CHECK(cv_img.data) << "Could not load image!";
        cv::TickMeter tm;
        tm.start();
        std::vector<vector<float> > detections = detector.Detect(cv_img);
        tm.stop();
        // draw detected box
        show_detection_box(cv_img, detections);
        // show fps
        if ( settings.show_fps && !show_fps(cv_img, tm.getTimeMilli()) ) {
          break;
        }
      }
    }
#endif  // USE_OPENCV_CAPTURE
    else {
      std::cerr << "Unknown file_type: " << settings.file_type << "\n";
    }

  remove_msgq();

  return 0;
}
#else
int main(int argc, char** argv) {
  LOG(FATAL) << "This example requires OpenCV; compile with USE_OPENCV.";
}
#endif  // USE_OPENCV
