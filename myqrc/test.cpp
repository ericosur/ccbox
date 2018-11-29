#include "test.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <quirc.h>

void crop_image(const cv::Mat& orig, cv::Mat& new_img, int x, int y, int w, int h)
{
    cv::Rect roi(x, y, w, h);
    new_img = orig(roi);
}

int test(std::string& out_string)
{
  using namespace cv;

  int ret = -1;
  cv::VideoCapture cap(0);

  if (!cap.isOpened()) {
    printf("cannot open video capture!\n");
    return -1;
  }

  const int width = 640;
  const int height = 480;

  //set height and width of capture frame
  cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
  cap.set(cv::CAP_PROP_FPS, 15);

  // int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  // int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

  struct quirc *qr = quirc_new();
  quirc_resize(qr, width, height);

  cv::namedWindow("window", cv::WINDOW_AUTOSIZE);
  cv::moveWindow("window", 0, 0);
  cv::Mat video_mat, grey_mat, grey_uchar_mat;

  while (true) {
    cap.read(video_mat);
    cvtColor(video_mat, grey_mat, cv::COLOR_BGR2GRAY);
    grey_mat.convertTo(grey_uchar_mat, CV_8U);

    uint8_t *qr_image = quirc_begin(qr, NULL, NULL);
    memcpy(qr_image, grey_uchar_mat.data, width*height);
    quirc_end(qr);

    int num_codes = quirc_count(qr);
    for (int i=0; i<num_codes; i++) {
      struct quirc_code code;
      quirc_extract(qr, i, &code);


      for (int j=0; j<4; j++) {
        struct quirc_point *a = &code.corners[j];
        struct quirc_point *b = &code.corners[(j + 1) % 4];
#ifdef OPENCV_IMSHOW
        line(video_mat, cv::Point(a->x, a->y), cv::Point(b->x, b->y), cv::Scalar(110, 220, 0), 2, 8);
#endif
        //printf("(%d) a: %d,%d  b: %d,%d\n", j, a->x, a->y, b->x, b->y);
      }

      struct quirc_data data;
      quirc_decode_error_t err = quirc_decode(&code, &data);
      if (!err) {
        //printf("payload: %s\n", (char*)data.payload);
        out_string = (char*)data.payload;
        ret = 0;
#ifdef OPENCV_IMSHOW
        // putText(video_mat, (char*)data.payload, cv::Point(code.corners[0].x, code.corners[0].y),
        //   cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(110, 220, 0));
#endif
      } else {
        //printf("err happens at quirc_decode\n");
      }
    }

    if (ret == 0) {
        const char fn[] = "quirc_result.png";
        imwrite(fn, video_mat);
        printf("output to %s\n", fn);
        break;
    }

#ifdef OPENCV_IMSHOW
    imshow("window", video_mat);
    int key = cv::waitKey(1);
    if (key == 0x1B) {
      break;
    }
#endif
  }

  quirc_destroy(qr);
  cap.release();
  printf("out_string: %s\n", out_string.c_str());
  return ret;
}

int try_tts(const std::string& str)
{
    using namespace std;
    const int cmd_max_length = 128;
    char cmd[cmd_max_length] = {0};

    cout << "try_tts: "  << str << "\n";
    snprintf(cmd, cmd_max_length, "node download.js \"%s\"", str.c_str());
    int r = system(cmd);
    return r;
}
