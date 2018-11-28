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

#include <pbox/pbox.h>
#include "cvutil.h"
#include "ssd_setting.h"
#include "MY_IPC.hpp"
#include "ssdutil.h"
#include "shmutil.h"
#include "detector.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <stdlib.h>
#include <signal.h>


//#define RASMUS_HACK

//using namespace caffe;  // NOLINT(build/namespaces)

//#define USE_DISTWIN
#ifdef USE_DISTWIN
#define DIST_WINDOW   "dist"
#endif

#define DETECTION_WIN "detections"



//#define OUTPUT_SSD_JSON    "/tmp/ssd.json"

//#define USE_MYIPC

bool showDebug = false;

void issue_dog_alert(int dist);
#ifdef USE_OLDALERT
void issue_man_alert(int dist);
bool issue_man_alert_v2(int dist);
bool issue_man_alert_v3(int dist, float score);
#endif
bool issue_man_alert_v4(int dist, float score, PersonRect& pr);
void reset_vol_level();

#define MIN_ACCEPTABLE_DIST     (50)
#define MAX_ACCEPTABLE_DIST     (550)
#define MAN_ALERT_SCORE         (0.8)

const int max_width = DEFAULT_WIDTH;
const int max_height = DEFAULT_HEIGHT;
const int rgb_byte = 3;
const int rgb_buffer_size = max_width*max_height*rgb_byte;
const int dep_buffer_size = max_width*max_height*2; // UINT16, 2bytes
uint8_t rgb_buffer[rgb_buffer_size];
uint8_t dep_buffer[dep_buffer_size];


int get_dpeth_pt(uint8_t* buffer, int x, int y);
int get_dpeth_pt2(uint8_t* dep_buffer, int x, int y);


void my_handle_ctrlc(int sig)
{
  printf("caught signal %d\n", sig);

  SsdSetting* s = SsdSetting::getInstance();
  if (s->wait_msgq) {
    remove_msgq();
  }

#ifdef USE_SOCKET
  if (s->sockfd >= 0) {
    send_string_to_server("quit");
  }
#endif

  // if (s->det != NULL) {
  //   delete s->det;
  // }

  exit(0);
  return;
}


#if 1
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
#endif

// rasmus hack, global variables
const int BUFFER_SIZE = 100;

//int fontface = cv::FONT_HERSHEY_SIMPLEX;
//double scale = 0.75;
//int thickness = 2;
int baseline = 0;

void show_distwin(float dist)
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
#endif
}

bool show_fps(cv::Mat& cv_img, double elapsed_time)
{
    using namespace cv;

    static double total_time = 0.0;
    static int frame_count = 0;
    static float fps = 0.0;
    char buffer[BUFFER_SIZE];
    const int max_frame_count = 30;
    SsdSetting* settings = SsdSetting::getInstance();

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
    Size text = getTextSize(buffer, fontface, scale, thickness, &baseline);
    rectangle(cv_img, Point(0, 0), Point(text.width, text.height + baseline),
          CV_RGB(255, 255, 255), FILLED);
    putText(cv_img, buffer, Point(0, text.height + baseline / 2.),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);

    if (settings->do_imshow) {
      cv::imshow(DETECTION_WIN, cv_img);
    } else {
      //printf("no show\n");
    }

    return true;
}


#ifdef USE_REALSENSE
float query_dist_from_detection_box(const std::vector< std::vector<float> >& detections,
                                    int img_cols, int img_rows)
{
  float tmp = 1000.0;
  SsdSetting* settings = SsdSetting::getInstance();

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
    if (score >= settings->confidence_threshold && (is_dog(label) || is_person(label))) {
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
#endif  // USE_REALSENSE


std::string show_detection_box(cv::Mat& cv_img,
                        bool hasCrop, cv::Mat& crop_img,
                        const std::vector< std::vector<float> >& detections,
                        int rx=0, int ry=0)
{
  using namespace cv;

  char buffer[BUFFER_SIZE] = {0};
  int detected = 0;
  bool hasDog = false;
  bool hasPerson = false;
  int dist = 0;
  SsdSetting* settings = SsdSetting::getInstance();
  const Scalar& color = Scalar( 255, 255, 255 );
  //PersonRect last_rect;
  std::vector<PersonRect> vv;
  // if (showDebug)
     //printf("%s +++\n", __func__);

  Mat orig_img = cv_img.clone();

  for (size_t i = 0; i < detections.size(); ++i) {

    const vector<float>& d = detections[i];
    // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
    CHECK_EQ(d.size(), 7);
    const int label = int(d[1]);
    const float score = d[2];

    int cols, rows;
    int box_x1, box_x2, box_y1, box_y2;

    if (hasCrop) {
      cols = crop_img.cols;
      rows = crop_img.rows;
    } else {
      cols = cv_img.cols;
      rows = cv_img.rows;
    }

    int x1 = static_cast<int>(d[3] * cols);
    int y1 = static_cast<int>(d[4] * rows);
    int x2 = static_cast<int>(d[5] * cols);
    int y2 = static_cast<int>(d[6] * rows);

    if (hasCrop) {
      box_x1 = rx + x1;
      box_x2 = rx + x2;
      box_y1 = ry + y1;
      box_y2 = ry + y2;
    } else {
      box_x1 = x1;
      box_x2 = x2;
      box_y1 = y1;
      box_y2 = y2;
    }

    bool hasDog = ( score >= settings->confidence_threshold && is_dog(label) );
    bool hasPerson = ( score >= MAN_ALERT_SCORE && is_person(label) );

    // if (hasPerson) {
    //   printf("ssd: hasPerson\n");
    // }

#ifdef USE_SOCKET
    check_recv();
#endif


    if ( hasDog || hasPerson ) {

      detected ++;
      // if (showDebug) {
         //printf("from: c%dr%d_%d %d %d %d\n", cols, rows, box_x1, box_x2, box_y1, box_y2);
      // }

      const int ww = box_x2 - box_x1;
      const int hh = box_y2 - box_y1;
      const int qx = box_x1 + ww / 2;
      const int qy = box_y1 + hh / 2;

      if (settings->direct_use_realsense && settings->file_type == "realsense") {
  #ifdef USE_REALSENSE
        //printf("fuck @%d\n", __LINE__);
        dist = pbox::get_dist_from_point(qx, qy);
          //dist = pbox::get_rs_dpeth_pt2(qx, qy);
        //printf("rs: dist: %d\n", dist);
  #else
        printf("USE_REALSENSE not set, dist will be always 0\n");
  #endif
      }
      else {  // using raw data to get depth
          if (ww > 10 && hh > 10 && qx > 10 && qy > 10) {
            dist = get_dpeth_pt2(dep_buffer, qx, qy);
          } else {
            printf("invalid range: w(%d)h(%d),qx(%d),qy(%d)\n", ww, hh, qx, qy);
            dist = 0;
          }
      }

      if (settings->do_dog_alert && hasDog) {
          issue_dog_alert(dist);
      }

    //printf("fuck @%d\n", __LINE__);

      if (hasPerson) {  // will not record down dog rect
        PersonRect pr(box_x1, box_y1, ww, hh, score, dist);
        //printf("@%d: (%d,%d,%d,%d) s(%.2f)\t", __LINE__, box_x1, box_y1, ww, hh, score);
        //printf("@%d: (%d,%d,%d,%d)", __LINE__, pr.get_rect().x, pr.get_rect().y, pr.get_rect().width, pr.get_rect().height);

        // could fetch depth data from realsense camera or IPC depth raw data
        if (dist > 0) {
          //printf("[%d] i:%d a:%d s(%.2f) d(%d)\r", __LINE__, (int)i, pr.get_area(), score, dist);
          vv.push_back(pr);
        } else {
          // still keep record of rect for person if webcam
          if (settings->file_type == "webcam") {
            vv.push_back(pr);
          }
        }


      }

      //pbox::mylog("ssd", "before draw_aim");
      draw_aim(cv_img, box_x1, box_y1, ww, hh);
      //show_distwin(dist);

      //draw box and text
      cv::Point top_left_pt(box_x1, box_y1);
      cv::Point bottom_right_pt(box_x2, box_y2);
      cv::Point bottom_left_pt(box_x1, box_y2);
      cv::rectangle(cv_img, top_left_pt, bottom_right_pt, color, 4);
      snprintf(buffer, sizeof(buffer), "%s: %d, %.2f",
               get_label_name(label).c_str(), dist, score);
      cv::Size text = cv::getTextSize(buffer, fontface, scale, thickness, &baseline);
      cv::rectangle(cv_img, bottom_left_pt + cv::Point(0, 0),
          bottom_left_pt + cv::Point(text.width, -text.height-baseline),
          color, FILLED);
      cv::putText(cv_img, buffer, bottom_left_pt - cv::Point(0, baseline),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
    } // if ( hasDog || hasPerson )
    //printf("for-loop --- @%d\t", __LINE__);

  } // for-loop

  // if (vv.size())
     //printf("vv.size():%d @%d -- %d\n", (int)vv.size(), __LINE__, settings->do_man_alert);

  if (settings->do_man_alert) {
    //printf("do man alert...\n");
    int idx = -1;
    if (vv.size() >= 1) {
      //printf("vv.size >= 1 @%d\t", __LINE__);
      idx = find_closest(vv);
      //idx = find_min_area(vv);
      //printf("%d: find_closest --- idx:%d\n", __LINE__, idx);
      if (idx != -1) {
        //printf("send cropped @%d\n", __LINE__);

        /**
        *** send cropped and resize image to reid server ***
        **/
#ifdef USE_SOCKET
        if (settings->do_reid && settings->bCouldSend) {
          send_crop_image_to_server(orig_img, vv[idx].get_rect());
        } else {
          //printf("bypass cropped images to reid server...\n");
        }
#else
        if (settings->do_reid) {
          save_crop_image(orig_img, vv[idx].get_rect());
        }
#endif
      }
      /* else {
        printf("idx == -1\n");
      }
      */
    }


    if (vv.size() > 0) {
      // update last volume epoch
      settings->set_vol_epoch();

#if 1
      if (idx == -1) {
        //printf("idx == %d, reset idx\n", idx);
        idx = 0;
      }

      if ( issue_man_alert_v4(vv[idx].get_dist(), vv[idx].get_score(), vv[idx]) ) {
        // deubg show
        if (settings->do_imshow && settings->show_debug) {
            cv::imshow("man_alert", cv_img);
            cv::waitKey(1);
        }
        //last_rect = vv[idx];
        //printf("[%d] last_rect: %d, vv: %d\n", __LINE__, last_rect.get_area(), vv[idx].get_area());
        //float iou = get_iou(aa, bb);
        //printf("[%d] iou: %.2f\n", __LINE__, iou);
      } // if issue out
#endif
    } // if (vv.size() > 0) {
  }
  vv.clear();

  if (strlen(buffer) && settings->show_debug) {
    printf("%s: %s\n", __func__, buffer);
  }

  if (abs(pbox::get_timeepoch() - settings->last_vol_epoch) > 3) {
    // long time no man
    reset_vol_level();
  } else {
    //printf("timer check: %d\n", pbox::get_timeepoch());
  }


  std::string r;
  if (hasDog) {
    r = "dog";
  }
  if (hasPerson) {
    r = "man";
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

int get_dpeth_pt(uint8_t* buffer, int x, int y)
{
  int res = 0;
  SsdSetting* settings = SsdSetting::getInstance();
  if ( (x < 0 || x >= DEFAULT_WIDTH) || (y < 0 || y >= DEFAULT_HEIGHT) ) {
    if (settings->show_debug)
      printf("%s: OOB %d,%d\n", __func__, x, y);
    res = 0;
  } else {
    uint16_t* pt = (uint16_t*)buffer;
    int cnt = y*DEFAULT_HEIGHT + x;
    res = (int)*(pt+cnt);
  }
  if (settings->show_debug)
    printf("%s: dist: %d\n", __func__, res);

  return res;
}

int get_avg(int* array, int array_size)
{
  int sum = 0;
  for (int i=0; i<array_size; ++i) {
    sum += array[i];
  }
  int avg = 0;
  if (array_size > 0) {
    avg = sum / array_size;
  }

  //fprintf(stderr, "get_avg = %d\n", avg);
  return avg;
}

int get_dpeth_pt2(uint8_t* buffer, int x, int y)
{
  int offset_x[] = {
    -6, -6, -6, -6, -6, -6,
    -4, -4, -4, -4, -4, -4,
    -2, -2, -2, -2, -2, -2,
     0,  0,  0,  0,  0,  0,
     2,  2,  2,  2,  2,  2,
     4,  4,  4,  4,  4,  4};
  int offset_y[] = {
    -5, -3, -1, 1, 3, 5,
    -5, -3, -1, 1, 3, 5,
    -5, -3, -1, 1, 3, 5,
    -5, -3, -1, 1, 3, 5,
    -5, -3, -1, 1, 3, 5,
    -5, -3, -1, 1, 3, 5,
  };

  // x,y as center point, take some points around it
  int cnt = 0;
  const int array_size = sizeof(offset_x) / sizeof(offset_x[0]);
  const int max_keep_size = array_size;
  int keep[max_keep_size] = {0};
  SsdSetting* settings = SsdSetting::getInstance();
  int tmp = 0;
  //const int threshold = 100;
  //int cdist = 0;
  if ( settings->show_debug ) {
    printf("%s: x:%d, y:%d\n", __func__, x, y);
  }

  memset(keep, 0, sizeof(int)*max_keep_size);
  for (int i=0; i<array_size; i++) {
//#ifdef USE_REALSENSE
    // #error do not use here
    // if (settings->direct_use_realsense) {
    //   int qx = x+offset_x[i];
    //   int qy = y+offset_y[i];
    //   tmp = pbox::get_dist_from_point(qx, qy);
    // }
//#else
    int pp = x + offset_x[i];
    int qq = y + offset_y[i];
    if (pp >= 0 && qq >= 0 && pp < DEFAULT_WIDTH && qq < DEFAULT_HEIGHT) {
      tmp = get_dpeth_pt(buffer, x+offset_x[i], y+offset_y[i]);
    } else {
      tmp = 0;
    }
//#endif
    if (tmp > 0 && tmp < MAX_ACCEPTABLE_DIST) {
      keep[cnt] = tmp;
      if (settings->show_debug)
      printf("tmp(%d)  ", tmp);
      cnt++;
    }
    if (cnt > max_keep_size) {
      if (settings->show_debug)
        printf("%s: reach max keep size: %d\n", __func__, cnt);
      break;
    }
  }

  int avg = get_avg(keep, cnt);

  return avg;
}

void issue_dog_alert(int dist)
{
  SsdSetting* settings = SsdSetting::getInstance();

  if (dist <= settings->dog_min_dist) {
    printf("issue_dog_alert: skip, dist<=%d\n", settings->dog_min_dist  );
    return;
  }
#ifdef USE_MYIPC
  //SsdSetting* settings = SsdSetting::getInstance();
  if (settings->wait_myipc) {
    if (settings->ipc != NULL) {
      settings->ipc->IPC_Put_TAG_INT32(settings->dog_warning_tag.c_str(), 1);
    }
  }
  std::cout << "=====>" << settings->dog_warning_tag << " !!!!!\n";
#else
  //
#endif
  pbox::mylog("ssd", "%s: dog detected@ %d !!!!", __func__, dist);
}

void swap(int& m, int& n)
{
  if (n < m) {
    int tmp = m;
    m = n;
    n = tmp;
  }
}

void reset_vol_level()
{
  SsdSetting* settings = SsdSetting::getInstance();
  int vol = settings->max_vol;
  if (settings->last_vol != vol) {
    printf("\n%s: reset vol level to %d\n", __func__, vol);
#ifdef USE_MYIPC
      if (settings->wait_myipc && settings->ipc != NULL) {
        settings->ipc->IPC_Put_TAG_INT32(settings->audience_vol_tag.c_str(), vol);
        settings->ipc->IPC_Put_TAG_INT32(settings->volume_adjust_tag.c_str(), 1);
      }
#endif  // USE_MYIPC
    settings->set_last_vol(vol);
    settings->set_last_dist(0);
  }
}


bool issue_man_alert_v4(int dist, float score, PersonRect& pr)
{
  bool v4_debug = false;
  // will skip if dist <= 60 or dist >= 550
  if (dist <= MIN_ACCEPTABLE_DIST || dist >= MAX_ACCEPTABLE_DIST) {
    if (v4_debug)
    printf("v4: oob ");
    return false;
  }
  SsdSetting* s = SsdSetting::getInstance();
  VolumeLevelEnum r = VOL_ZERO;
  static PersonRect record;
  float iou = get_iou(record.get_rect(), pr.get_rect());
  bool isJumped;
  int state = 0;
  static int last_state = 0;

  if (dist != 0 && s->last_dist == 0) {
    isJumped = true;
    if (v4_debug)
    printf("v4: j ");
    goto get_volume_label;
  }

  //printf("v4: %s d(%d), s(%.2f), vol(%d)+++  ", __func__, dist, score, s->last_vol);
  if (dist == s->last_dist)  {  // not change
    if (v4_debug)
    printf("v4: s ");
    return false;
  }

  // NOTICE HERE, WILL LOOSE THE THRESHOLD TO 0.8
  if (score > s->last_score || score > 0.8) {
    //if (s->show_debug)
    //printf("update dist: %d\r", dist);
    s->skipped = 0;
    s->last_score = score;
  }
  else if (s->last_dist != 0) {
    if (abs(dist - s->last_dist) > s->threshold) {
      //if (s->show_debug)
      //printf("skip too large %d vs %d s(%.2f)\n", dist, s->last_dist, s->last_score);
      if (v4_debug)
      printf("n ");
      s->skipped ++;
      return false;
    }
  }

  //printf("issue_man_alert_v4: iou: %.2f\n", iou);
  if (iou > 0.6) { // most area is same...
    //s->set_last_dist(dist);
    if (v4_debug)
    printf("i ");
    return false;
  }

get_volume_label:
  // if (isJumped) {
  //   s->last_dist = dist;
  // }

  //s->last_score = 0.9;
  if (dist > s->last_dist) { // increasing...
    //printf("issue_man_alert_v3: dist is increasing...\n");
    r = get_vol_while_increase(dist);
    //printf(" inc(%d):%d ", dist, r);
    //printf("1 ");
    state = 1;
  } else {
    //printf("issue_man_alert_v3: dist is decreasing...\n");
    r = get_vol_while_decrease(dist);
    //printf(" dec(%d):%d ", dist, r);
    //printf("-1 ");
    state = -1;
  }

  int vol = s->volumes[r];
  if ( vol <= 0 ) {
    return false;
  }


  //if ( (state == 1 && last_state == -1) || (state == -1 && last_state == 1) ) {
    if ( abs(dist - s->last_dist) < (s->minus_offset + s->plus_offset) ) {
      //printf("%d %d ", dist, s->last_dist);
      last_state = state;
      return false;
    }
  //}

  s->set_last_dist(dist);

  // vol changes
  if ( vol != s->last_vol ) {
    s->set_last_vol(vol);  // record the epoch that changes vol
#ifdef USE_MYIPC
    if (s->wait_myipc && s->ipc != NULL) {
      s->ipc->IPC_Put_TAG_INT32(s->audience_vol_tag.c_str(), vol);
      s->ipc->IPC_Put_TAG_INT32(s->volume_adjust_tag.c_str(), 1);
    }
#endif  // USE_MYIPC
    {
      printf("\n====> issue_man_alert_v4: PUT %s/%d d(%d) s(%.2f) iou(%.2f)\n",
        s->audience_vol_tag.c_str(), vol, dist, s->last_score, iou);
    }

    record = pr;
    //printf("iou: %.2f\n", get_iou(record.get_rect(), pr.get_rect()));
    return true;
  } else {
    return false;
  }

}


#ifdef USE_OLDALERT
bool issue_man_alert_v3(int dist, float score)
{
  // will skip if dist <= 60 or dist >= 550
  if (dist <= MIN_ACCEPTABLE_DIST || dist >= MAX_ACCEPTABLE_DIST) {
    return false;
  }
  SsdSetting* s = SsdSetting::getInstance();
  VolumeLevelEnum r = VOL_ZERO;

  //printf("%s d(%d), s(%.2f), vol(%d)+++\n", __func__, dist, score, s->last_vol);
  if (dist == s->last_dist)  {  // not change
    //printf("issue_man_alert_v3: dist not change...\n");
    return false;
  }
  if (score > s->last_score) {
    //if (s->show_debug)
    //printf("update dist: %d\r", dist);
    s->skipped = 0;
    s->last_score = score;
  }
  else if (s->last_dist != 0) {
    if (abs(dist - s->last_dist) > s->threshold) {
      //if (s->show_debug)
      //printf("skip too large %d vs %d s(%.2f)\n", dist, s->last_dist, s->last_score);
      printf("skip...");
      s->skipped ++;
      return false;
    }
  }
  //s->last_score = 0.9;
  if (dist > s->last_dist) { // increasing...
    //printf("issue_man_alert_v3: dist is increasing...\n");
    r = get_vol_while_increase(dist);
  } else {
    //printf("issue_man_alert_v3: dist is decreasing...\n");
    r = get_vol_while_decrease(dist);
  }

  int vol = s->volumes[r];
  if ( vol <= 0 ) {
    return false;
  }

  s->set_last_dist(dist);

  // vol changes
  if ( vol != s->last_vol ) {
    s->set_last_vol(vol);  // record the epoch that changes vol
#ifdef USE_MYIPC
    if (s->wait_myipc && ipc != NULL) {
      ipc->IPC_Put_TAG_INT32(s->audience_vol_tag.c_str(), vol);
      ipc->IPC_Put_TAG_INT32(s->volume_adjust_tag.c_str(), 1);
    }
#endif  // USE_MYIPC
    {
      printf("\n===> man_alert_v3: PUT %s/%d d(%d) s(%.2f)\n",
        s->audience_vol_tag.c_str(), vol, dist, s->last_score);
    }
    return true;
  } else {
    return false;
  }

}
#endif

#ifdef USE_OLDALERT
// dist is distance in cm
// three level of distance mapping to sound volume (150/250/350)
// fuzzy range
bool issue_man_alert_v2(int dist)
{
  // will skip if dist <= 0
  if (dist <= 0 || dist >= 550) {
    return false;
  }

  SsdSetting* settings = SsdSetting::getInstance();

  const int low_vol = settings->low_vol;
  const int mid_vol = settings->mid_vol;
  const int high_vol = settings->high_vol;
  const int max_vol = settings->max_vol;
  const int threshold = settings->threshold;
  const int pt1 = settings->pt1;
  const int pt2 = settings->pt2;
  const int pt3 = settings->pt3;
  const int bypass_limit = 5;

  // take it as no change
  if ( abs(settings->last_dist - dist) < threshold
       && settings->pass_threshold < bypass_limit) {

    // bypass
    settings->pass_threshold ++;

    if (settings->pass_threshold == 1) {
      printf("man_alert_v2: threshold, dist/last: %d vs %d v(%d)\n",
        dist, settings->last_dist, settings->last_vol
        /*, settings->pass_threshold*/
      );
    }

    settings->set_last_vol(settings->last_vol);
    if (settings->last_vol != 0) {
      return false;
    }

  } else {
    printf("man_alert_v2: check VOL, dist/last: %d vs %d v(%d)\n",
      dist, settings->last_dist, settings->last_vol);
    settings->set_last_dist(dist);
  }

  settings->pass_threshold = 0;

  int vol = 0;

  if (dist < pt1) {
    vol = low_vol;
  } else if ( dist >= pt1 && dist < pt2 ) {
    vol = mid_vol;
  } else if ( dist >= pt2 && dist < pt3 ) {
    vol = high_vol;
  } else if (dist > pt3) {
    vol = max_vol;
  }

  if (vol != 0 && vol != settings->last_vol) {
    settings->set_last_vol(vol);  // record the epoch that changes vol
#ifdef USE_MYIPC
    if (settings->wait_myipc && ipc != NULL) {
      ipc->IPC_Put_TAG_INT32(settings->audience_vol_tag.c_str(), vol);
      ipc->IPC_Put_TAG_INT32(settings->volume_adjust_tag.c_str(), 1);
    } else
#endif  // USE_MYIPC
    {
      printf("man_alert_v2: PUT %s/%d dist(%d)\n",
        settings->audience_vol_tag.c_str(), vol, dist);
    }
    return true;
  } else {
    // printf("alert_v2: vol no change, last_vol:%d\n",
    //     settings->last_vol);
    settings->set_last_vol(settings->last_vol);
  }

  return false;
}
#endif

#ifdef USE_OLDALERT
// dist in cm
// need smooth the volume adjust value
void issue_man_alert(int dist)
{
  static int old_vol = 0;
  static int last_ratio = 0;
  const int level = 50;
  const int max_ratio = 9;
  const int threshold = 20;
  const int vol_per_ratio = SsdSetting::getInstance()->vol_per_ratio;

  if (dist <= 0) {
    return;
  }

  int vol = 0;
  int ratio = dist / level + 1;  // 1,2,3,4,5,6,7,8,...

  if (ratio >= max_ratio) {
    ratio = max_ratio;
  }

  if (last_ratio == 0) {
    // assign it
    last_ratio = ratio;
  }

  if (last_ratio - ratio >= 2) {
    ratio ++;
  }
  if (ratio - last_ratio >= 2) {
    ratio --;
  }

  vol = ratio * vol_per_ratio + 5;

  if (dist > 0 && vol > 0) {
    pbox::mylog("man_alert", "d:%d/r:%d/v:%d/ov:%d ",
                dist, ratio, vol, old_vol);
  }

  if (old_vol == 0) {
    old_vol = vol;
  }
  if (vol > 60 && vol - old_vol > threshold) {
    if (vol > 15) {
      vol -= 15;
    }
  }

  //}
#ifdef USE_MYIPC
  SsdSetting* settings = SsdSetting::getInstance();
  if (settings->wait_myipc) {
    if (settings->ipc != NULL && vol != 0 && vol != old_vol) {
      settings->ipc->IPC_Put_TAG_INT32(settings->audience_vol_tag.c_str(), vol);
      settings->ipc->IPC_Put_TAG_INT32(settings->volume_adjust_tag.c_str(), 1);
    }
  } else {
    printf("dgb: put %s/%d", settings->audience_vol_tag.c_str(), vol);
  }
#endif  // USE_MYIPC

  old_vol = vol;
}
#endif

int main(int argc, char** argv)
{
#ifdef RASMUS_HACK
  printf("hello world\n");
#else  // RASMUS_HACK

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
  SsdSetting* settings = SsdSetting::getInstance();

  signal(SIGINT, my_handle_ctrlc);

  system("mkdir -p /tmp/reid/");
  system("rm -f /tmp/reid/*");

  if (argc == 1) {
    printf("ssd_detect built at: %s %s", __DATE__, __TIME__);
    return 0;
  }

  if (argc == 2) {
    pbox::mylog("ssd", "use %s as setting file\n", argv[1]);
    if ( settings->read_values_from_json(argv[1]) ) {
      pbox::mylog("ssd", "read settings finished\n");
    } else {
      pbox::mylog("ssd", "read settings failed\n");
      return __LINE__;
    }
  } else if (argc < 7) {
    //gflags::ShowUsageWithFlagsRestrict(argv[0], "examples/ssd/ssd_detect");
    pbox::mylog("ssd", "insufficient number of arguments\n");
    return __LINE__;
  } else {
    // will use arguments from cli
    if ( settings->fill_values(argc, argv) ) {
      pbox::mylog("ssd", "%s\n", "settings.fill_values ok");
    } else {
      pbox::mylog("ssd", "%s\n", "settings.fill_values failed");
      return __LINE__;
    }
  }

  const string& mean_file = FLAGS_mean_file;
  const string& mean_value = FLAGS_mean_value;

  showDebug = (settings->show_debug != 0);

  pbox::mylog("ssd", "start init Detector...\n");
  // Initialize the network.
  Detector* det = new Detector(settings->model_file, settings->weights_file, mean_file, mean_value);
  settings->det = det;

#ifdef USE_SOCKET
  setup_connect();
#endif

  //pbox::mylog("ssd", "end init Detector...\n");
  //settings->recordlog("end init Detector...\n");

#ifdef USE_MYIPC
    if (settings->wait_myipc) {
      settings->ipc = MY_IPC::MY_IPC_GetInstance();
      if (settings->ipc == NULL) {
        fprintf(stderr, "!!! Failed to init MY_IPC\n");
        return 1;
      }
    }
#endif

    if (settings->file_type == "image") {

      // Process image one by one.
      cv::Mat p;
      while (true) {
        cv::Mat cv_img = cv::imread(argv[7], -1);
        CHECK(!cv_img.empty()) << "Unable to decode image " << argv[7];
        cv::TickMeter tm;
        tm.start();
        std::vector<vector<float> > detections = det->Detect(cv_img);
        tm.stop();
        // draw detected box
        show_detection_box(cv_img, false, p, detections, false);
        //show fps
        if ( settings->show_fps && !show_fps(cv_img, tm.getTimeMilli()) ) {
          break;
        }
      }
    }
    else if (settings->file_type == "realsense") {
      //cv::Mat cv_img = cv::Mat::zeros(640,480,CV_8UC3);
      //remove_shm();


      const int max_fnlen = 256;
      char rgbfn[max_fnlen];
      char depfn[max_fnlen];
      std::string cmd;
      std::string ofn;
      const std::string default_img_fn = "/tmp/rs.png";


      //uint32_t readsize = 0;
      //byte buffer[MAX_BUFFER_SIZE];
      std::vector<std::string> v;
      //int img_idx = 0;
      using namespace pbox;


#ifdef USE_REALSENSE
      bool hasRealsenseOn = false;
#endif

      while (true) {
        cv::Mat cv_img;

        if (settings->direct_use_realsense) {
          //cmd = "realsense";
        } else {

          if (settings->wait_msgq) { // use msgq instead of MY_IPC
            cmd = wait_msgq();
            // TODO: make dir configurable
            if (settings->testraw) {
              snprintf(rgbfn, max_fnlen, "%s/rgb%s.bin", settings->rawbin_dir.c_str(), cmd.c_str());
              snprintf(depfn, max_fnlen, "%s/depth%s.bin", settings->rawbin_dir.c_str(), cmd.c_str());
            } else {
              // will use full file path
              snprintf(rgbfn, max_fnlen, "%s", cmd.c_str());
            }

            printf("wait_msgq got: %s\n%s\n", rgbfn, depfn);
            v.clear();
            v.push_back("wait for msgq");

          }
#ifdef USE_MYIPC
          else if (settings->wait_myipc) { // use MY IPC as IPC
            cmd = "myipc";
            int img_idx = 0;
            // will block here
            if (settings->ipc != NULL) {
              img_idx = settings->ipc->IPC_Get_TAG_INT32(settings->wait_myipc_tag.c_str(), 0);
              snprintf(rgbfn, sizeof(rgbfn), "%s/rgb%d.bin", settings->rawbin_dir.c_str(), img_idx);
              snprintf(depfn, sizeof(depfn), "%s/depth%d.bin", settings->rawbin_dir.c_str(), img_idx);
            }
            if (showDebug) {
              printf("wait_myipc: got %d\tfn: %s\t%s\n", img_idx, rgbfn, depfn);
            }
          }
#endif  // USE_MYIPC
          //printf("[%d] IPC cmd: %s\n", __LINE__, cmd.c_str());
          //printf("here rgbfn: %s\ndepfn: %s\n", rgbfn, depfn);
        }

        if (cmd == "quit" || cmd == "exit") {
          printf("exit...\n");
          v.push_back("got quit @" + pbox::get_timestring());
          break;
        }
#ifdef USE_REALSENSE
        else if (settings->direct_use_realsense) {
          //v.clear();
          //v.push_back(cmd);

          if (!hasRealsenseOn) {
            printf("turn on realsense and use it...\n");
            //v.push_back("turn on realsense @" + pbox::get_timestring());
          }
          pbox::init_realsense();
          //printf("pbox::init_realsense() ---\n");
          hasRealsenseOn = true;
          //v.push_back("hasRealsenseOn");

          pbox::get_color_mat_from_realsense(cv_img);
          //printf("pbox::get_color_mat_from_realsense() ---\n");
          //pbox::output_status(OUTPUT_SSD_JSON, v);
        }
#endif// USE_REALSENSE
        else if (pbox::is_file_exist(rgbfn)) {
          //printf("rgbfn: %s\n", rgbfn);
          //v.clear();
          //v.push_back("read from " + std::string(rgbfn));
          if (settings->wait_myipc || settings->testraw) {
            if (settings->show_debug)
              printf("myipc or testraw\n");
            load_bin_to_buffer(rgbfn, rgb_buffer, rgb_buffer_size);
            cv_img = cv::Mat(cv::Size(max_width, max_height), CV_8UC3, (void*)rgb_buffer);
            cv::cvtColor(cv_img, cv_img, cv::COLOR_RGB2BGR);
            load_bin_to_buffer(depfn, dep_buffer, dep_buffer_size);
            //imshow("wait_msgq", cv_img);
            cv::waitKey(500);
          } else if (settings->wait_msgq) {
            printf("read image from: %s\n", rgbfn);
            cv_img = cv::imread(cmd);
          }
#if 0
          if (settings->show_debug) {
            imshow("press to continue", cv_img);
            int r = get_dpeth_pt(dep_buffer, max_width/2, max_height/2);
            (void)r;
            //printf("r = %d\n", r);
            //cv::waitKey(1000);
          }
#endif
          ofn = cmd + ".json";
          cmd = "";

        } else {
          printf("file not found: %s\n", rgbfn);
          //pbox::output_status(OUTPUT_SSD_JSON, v);
          continue;
        }

        cv::TickMeter tm;
        int img_try = 0;
        const cv::Mat original_img = cv_img;
        int rx = 0;
        int ry = 0;
        bool isCrop = false;

        do {
          // if (showDebug && isCrop) {
          //   printf("CROPPED!!! size: %d x %d\n", cv_img.cols, cv_img.rows);
          // }

          //printf("before detector +++\n");
          tm.start();
          std::vector<vector<float> > detections = det->Detect(cv_img);
          tm.stop();
          //printf("detector ---\n");

#ifdef USE_SOCKET
          check_recv();
#endif
          // temmp test
          //imshow("temp", cv_img);

          // show detection result
          std::string r;
          {
            // use gui to show results
            cv::Mat result_img = original_img.clone();
            if (isCrop) {
              cv::Mat partial_img = cv_img.clone();
              //printf("=====> rx:%d ry:%d\n", rx, ry);
              show_detection_box(result_img, isCrop, partial_img, detections, rx, ry);
              rx = 0;
              ry = 0;
              isCrop = false;
              // if (settings->do_imshow) {
              //   imshow("cooo", cv_img);
              // }
            } else {
              // detect from original image
              cv::Mat p;
              show_detection_box(result_img, false, p, detections);
            }

            if (settings->do_imshow) {
              imshow(DETECTION_WIN, result_img);
            }
            // show fps
            if ( settings->show_fps) {
              show_fps(result_img, tm.getTimeMilli());
            }
          }

          // sart try small object detection
          if (showDebug)
            pbox::mylog("ssd", "img_try: %d", img_try);
          img_try ++;
          isCrop = true;
          crop_image(original_img, cv_img, rx, ry, img_try,
                           false /*settings.do_imshow*/);

        } while ( img_try < settings->max_crop_try );

        //query_dist_from_detection_box(detections, cv_img.cols, cv_img.rows);

        int k = cv::waitKey(1);
        if (k == 27 || k == 'q') {
          break;
        }
      } // while-loop

      //v.push_back("break at " + pbox::get_timestring());
      //pbox::output_status(OUTPUT_SSD_JSON, v);

    } // REALSENSE if (settings.file_type == "realsense")
#ifdef USE_OPENCV_CAPTURE
    else if (settings->file_type == "webcam") {
      cv::VideoCapture cap;
      if (!cap.open(settings->video_id)) {
        LOG(FATAL) << "Failed to open webcam: " << settings->video_id;
        CV_Assert("Cam open failed");
      }
      pbox::mylog("webcam", "before setting capturing device");
      cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
      cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
      cap.set(cv::CAP_PROP_AUTOFOCUS, 0);
      cap.set(cv::CAP_PROP_FPS, 30);
      for(;;) {
        cv::Mat cv_img;
        cap >> cv_img;
        CHECK(cv_img.data) << "Could not load image!";
        cv::TickMeter tm;
        tm.start();
        std::vector<vector<float> > detections = det->Detect(cv_img);
        tm.stop();
        // draw detected box
        cv::Mat p;
        show_detection_box(cv_img, false, p, detections);
        // show fps
        if ( settings->show_fps && !show_fps(cv_img, tm.getTimeMilli()) ) {
          break;
        }
        if ( settings->do_imshow ) {
          imshow(DETECTION_WIN, cv_img);
          int key = cv::waitKey(1);
          if ( key == 27 || key == 'q' ) {
            break;
          } else {
            // do nothing...
          }
        }
      }
    }
#endif  // USE_OPENCV_CAPTURE
    else {
      std::cerr << "Unknown file_type: " << settings->file_type << "\n";
    }

  if (settings->wait_msgq) {
    remove_msgq();
  }

#endif  // RASMUS_HACK

  return 0;
}
