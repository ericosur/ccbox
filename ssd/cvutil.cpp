#include "cvutil.h"
#include "ssd_setting.h"
#include "MY_IPC.hpp"
#include <pbox/pbox.h>

#include <cstdlib>
#include <iostream>
#include <ctime>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#define DEFAULT_CROP_WIDTH      240
#define DEFAULT_CROP_HEIGHT     240

using namespace std;
using namespace cv;

const cv::Scalar white = cv::Scalar(255, 255, 255);

int PersonRect::count = 0;

uint8_t* read_image(const string fn, size_t* size)
{
    Mat tstMat = imread(fn);
    bool _debug = false;

    vector<uint8_t> inImage;
    imencode(".jpg", tstMat, inImage);
    size_t datalen = inImage.size();
    if (_debug)
        cout << "datalen: " << datalen << "\n";

    uint8_t* buffer = (uint8_t*)malloc(inImage.size());
    if (buffer == NULL) {
        printf("buffer is null, cannot allocate...\n");
        return NULL;
    }
    *size = inImage.size();
    std::copy(inImage.begin(), inImage.end(), buffer);
    //free(buffer);

    if (_debug)
        cout << "read into buffer: " << fn << endl;
    return buffer;
}

#ifdef USE_SOCKET

// refer to:
// http://beej-zhtw.netdpi.net/09-man-manual/9-13-inet_ntoa-inet_aton-inet_addr

void ljust(char* str, size_t filllen)
{
    size_t len = strlen(str);
    memset(&str[len], 0x20, filllen - len);
}

////////// client/server function
int send_crop_image_to_server(const cv::Mat& orig_img, const cv::Rect& rr)
{
    const int BUFFER_SIZE = 384;
    char buffer[BUFFER_SIZE];
    cv::Mat crop_img;

    // crop image with specified rectangle
    crop_image_rect(orig_img, crop_img, rr);

    // resize image to limited 64x128
    cv::Mat resized_img(128, 64, CV_8UC3);

    cv::resize(crop_img, resized_img, resized_img.size(), 0, 0);
    snprintf(buffer, BUFFER_SIZE, "/tmp/reid/r%04d.jpg", PersonRect::get_count());
    PersonRect::inc_count();
    //printf("output to %s\n", buffer);
    imwrite(buffer, resized_img);

    transfer_image_to_server(buffer);

    return 0;
}
////////// client/server function

void setup_connect()
{
    SsdSetting* sett = SsdSetting::getInstance();
    if (!sett->do_reid) {
        return;
    }

    printf("%s\n", __func__);

    int sockfd;
    struct sockaddr_in dest;
    int ret;

    printf("try setup_connect\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sett->sockfd = sockfd;
    if (sockfd < 0) {
        perror("socket error: ");
        return;
    }

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(sett->port);
    ret = inet_aton(sett->host.c_str(), &dest.sin_addr);
    if (ret < 1) {
        perror("inet_aton fail: ");
    }
    connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
    printf("setup_connect: connected...\n");
}

ReidName check_recv()
{
    SsdSetting* sett = SsdSetting::getInstance();
    if (!sett->do_reid) {
        //printf("function do_reid off\n");
        return kNone;
    }

#ifdef USE_MYIPC
      if (sett->wait_myipc) {
        if (sett->ipc == NULL) {
            sett->ipc = MY_IPC::MY_IPC_GetInstance();
        }
        if (sett->ipc == NULL) {
          fprintf(stderr, "!!! Failed to init MY_IPC\n");
        }
      }
#endif

    int sockfd = sett->sockfd;
    const int RECVBUF_SIZE = 128;
    char recv_buf[RECVBUF_SIZE];
    char result[RECVBUF_SIZE];

    if (sockfd < 0) {
        printf("socket error\n");
        return kNone;
    }

    bzero(result, RECVBUF_SIZE);
    bzero(recv_buf, RECVBUF_SIZE);
    int ret = recv(sockfd, recv_buf, RECVBUF_SIZE, MSG_DONTWAIT);
    if (ret == -1) {
        return kNone;
    }
    //printf("ret of recv: %d, %s\n", ret, recv_buf);
    char* pch = strstr(recv_buf,":");
    if (pch != NULL) {
        ReidName rn = kNone;
        strncpy(result, pch+1, 32);
        //printf("%s: result: %s\n", __func__, result);
        sett->bCouldSend = true;
        // TODO: use MY_IPC to send out the name
        if (strcmp(result, "bin") == 0) {
            rn = kBin;
            sett->ipc->IPC_Put_TAG_INT32("bin", 1);
            printf("detect bin\n");
        } else if (strcmp(result, "allen") == 0) {
            rn = kAllen;
            sett->ipc->IPC_Put_TAG_INT32("allen", 1);
            printf("detect allen\n");
        } else if (strcmp(result, "rasmus") == 0) {
            rn = kRasmus;
            printf("detect rasmus\n");
        } else if (strcmp(result, "joe") == 0) {
            rn = kJoe;
        } else if (strcmp(result, "matched") == 0) {
            rn = kPerson;
        }
        return rn;
    }

    // if (strcmp(recv_buf, "ok") == 0) {
    //     sett->bCouldSend = true;
    // }
    return kNone;
}

void send_string_to_server(const std::string& msg)
{
    SsdSetting* sett = SsdSetting::getInstance();

    if (!sett->do_reid) {
        return;
    }

    int sockfd = sett->sockfd;
    int ret;
    //bool _debug = false;

    if (sockfd < 0) {
        perror("socket error: ");
        return;
    }

    int size = (int)msg.length();

    const int SIZEBUF_SIZE = 16;
    char size_buf[SIZEBUF_SIZE];

    snprintf(size_buf, SIZEBUF_SIZE, "%d", (int)size);
    //cout << "size: " << size << "\n";
    ljust(size_buf, SIZEBUF_SIZE);
    //mytool::dump(size_buf, SIZEBUF_SIZE);

    ret = send(sockfd, size_buf, SIZEBUF_SIZE, 0);
    //cout << "ret: " << ret << endl;
    if (ret != SIZEBUF_SIZE) {
        cout << "ret != SIZEBUF_SIZE\n";
    }

    ret = send(sockfd, msg.c_str(), size, 0);
    if (ret < 0) {
        perror("send error...");
    }
    if (ret != (int)size) {
        printf("ret(%d) != size(%d)\n", (int)ret, (int)size);
    }
}

void transfer_image_to_server(const std::string& file)
{
    SsdSetting* sett = SsdSetting::getInstance();
    if (!sett->do_reid) {
        return;
    }
    int sockfd = sett->sockfd;
    int ret;
    bool _debug = false;

    if (sockfd < 0) {
        perror("socket error: ");
        return;
    }

    const int SIZEBUF_SIZE = 16;
    char size_buf[SIZEBUF_SIZE];
    size_t size;
    uint8_t* buffer = read_image(file, &size);

    if (buffer == NULL) {
        printf("buffer is NULL\n");
        return;
    }

    if (_debug) {
        printf("sockfd: %d: prepare to send: %d bytes\n", sockfd, (int)size);
    }

    snprintf(size_buf, SIZEBUF_SIZE, "%d", (int)size);
    //cout << "size: " << size << "\n";
    ljust(size_buf, SIZEBUF_SIZE);
    //mytool::dump(size_buf, SIZEBUF_SIZE);

    ret = send(sockfd, size_buf, SIZEBUF_SIZE, 0);
    //cout << "ret: " << ret << endl;
    if (ret != SIZEBUF_SIZE) {
        cout << "ret != SIZEBUF_SIZE\n";
    }

    ret = send(sockfd, buffer, size, 0);
    if (ret < 0) {
        perror("send error...");
    }
    if (ret != (int)size) {
        printf("ret(%d) != size(%d)\n", (int)ret, (int)size);
    }
    free(buffer);
    sett->bCouldSend = false;

    //close(sockfd);
}
#endif  // #ifdef USE_SOCKET


int save_crop_image(const cv::Mat& orig_img, const cv::Rect& rr)
{
    const int FN_SIZE = 128;
    char fn[FN_SIZE];
    static int cnt = 0;

    cv::Mat crop_img;
    SsdSetting* sett = SsdSetting::getInstance();

    // crop image with specified rectangle
    crop_image_rect(orig_img, crop_img, rr);
    // resize image to limited 64x128
    cv::Mat resized_img(128, 64, CV_8UC3);
    cv::resize(crop_img, resized_img, resized_img.size(), 0, 0);

    // prepare filename for reid
    bzero(fn, sizeof(fn));
    snprintf(fn, FN_SIZE, "/tmp/reid%04d.jpg", cnt);
    //printf("cropped image output to %s\n", fn);
    imwrite(fn, resized_img);
    cnt ++;
    if (cnt > 33) {
        cnt = 0;
    }

#ifdef USE_MYIPC
      if (sett->wait_myipc) {
        if (sett->ipc == NULL) {
            sett->ipc = MY_IPC::MY_IPC_GetInstance();
        }
        if (sett->ipc == NULL) {
          fprintf(stderr, "!!! Failed to init MY_IPC\n");
          return -1;
        }
        sett->ipc->IPC_Put_TAG_String("reid_jpg_path", fn);
      }
#endif

    return 0;
}


void draw_aim(cv::Mat& img, int x, int y, int w, int h)
{
    using namespace cv;

    const int radius = 30;
    const int tail = 5;
    int cx = x + w/2;
    int cy = y + h/2;

    circle(img, Point(cx, cy), radius, white, thickness);
    line(img, Point(cx-radius-tail, cy), Point(cx+radius+tail, cy), white, thickness);
    line(img, Point(cx, cy-radius-tail), Point(cx, cy+radius+tail), white, thickness);
}

void draw_aim(cv::Mat& img, int w, int h)
{
    using namespace cv;

    const int radius = 30;
    const int tail = 5;
    int cx = w/2;
    int cy = h/2;

    circle(img, Point(w/2, h/2), radius, white, thickness);
    line(img, Point(cx-radius-tail, cy), Point(cx+radius+tail, cy), white, thickness);
    line(img, Point(cx, cy-radius-tail), Point(cx, cy+radius+tail), white, thickness);
}


void draw_dist(cv::Mat& img, float dist)
{
    using namespace cv;

    const int buffer_size = 128;
    using namespace cv;
    char buffer[buffer_size];
    int baseline = 0;

    snprintf(buffer, buffer_size, "dist: %.3fm", dist);
    Size text = cv::getTextSize(buffer, fontface, scale, thickness, &baseline);
    rectangle(img, cv::Point(0, 0), Point(text.width, text.height + baseline),
          CV_RGB(255, 255, 255), FILLED);
    putText(img, buffer, Point(0, text.height + baseline / 2.),
          fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

void crop_image(const cv::Mat& orig, cv::Mat& new_img, int x, int y, int w, int h)
{
    using namespace cv;

    cv::Rect roi(x, y, DEFAULT_CROP_WIDTH, DEFAULT_CROP_HEIGHT);
    new_img = orig(roi);
}

void crop_image_rect(const cv::Mat& orig, cv::Mat& new_img, cv::Rect rr)
{
    //printf("orig: col:%d, row:%d\n", orig.cols, orig.rows);
    cv::Rect nn = rr;
    if (nn.x < 0) {
        nn.x = 0;
    }
    if (nn.y < 0) {
        nn.y = 0;
    }
    if (nn.x + nn.width > DEFAULT_WIDTH) {
        nn.width = DEFAULT_WIDTH - nn.x;
    }
    if (nn.y + nn.height > DEFAULT_HEIGHT) {
        nn.height = DEFAULT_HEIGHT - nn.y;
    }

    new_img = orig(nn);
}

void crop_image(const cv::Mat& orig, cv::Mat& new_img, int& rx, int& ry,
    //int& min_crop_width, int& min_crop_height,
    int idx = 0,
    bool do_imshow=false)
{
    static bool isInited = false;
    SsdSetting* settings = SsdSetting::getInstance();

    if (!isInited) {
        std::srand(std::time(nullptr)); // use current time as seed for random generator
        isInited = true;
    } else {
        // no need to srand again;
    }

    const int base_x = 80;
    const int base_y = 230;
    int min_crop_width = 220;
    const int min_crop_height = 90;

    //rx = base_x + std::rand() % 320;
    rx = base_x + idx * 80;
    ry = base_y;

    if (settings->show_debug) {
        printf("%s: rx:%d,ry:%d\n", __func__, rx, ry);
    }

    // if (settings->show_debug) {
    //     std::cout << "rx/ry:" << rx << ", " << ry
    //         << "//" << rx+min_crop_width << "," << ry + min_crop_height << "\n";
    //     std::cout << "min_crop_width:" << min_crop_width << "," << min_crop_height << "\n";
    // }

    crop_image(orig, new_img, rx, ry, min_crop_width, min_crop_height);

    //if (do_imshow) {
        //imshow("cooo", new_img);
    //}
}

bool in_the_range_of(const int pt, const int target, const int threshold)
{
  int min = target - threshold;
  int max = target + threshold;
  if (min < 0) {
    min = 0;
    return false;
  }
  if (pt > min && pt < max) {
    return true;
  }
  return false;
}

float get_iou(const cv::Rect& a, const cv::Rect& b)
{
    cv::Rect unionRect = a | b;
    cv::Rect intersectionRect = a & b;
    if (unionRect.area() == 0) {
        return 0.0;
    }
    float iou = (float)intersectionRect.area() / (float)unionRect.area();
    return iou;
}

int find_min_area(const std::vector<PersonRect> &v)
{
  int last_idx = -1;
  int last_min_area = -1;

  if (v.size() > 0) {
    last_min_area = v[0].get_area();
    last_idx = 0;
  } else if (v.size() == 1) {
    return 0;
  }

  for (int ii=1; ii<v.size(); ++ii) {
    if (v[ii].get_area() < last_min_area) {
      last_idx = ii;
      last_min_area = v[ii].get_area();
    }
  }

  return last_idx;
}

int find_max_area(const std::vector<PersonRect> &v)
{
  int last_idx = -1;
  int last_max_area = -1;

  if (v.size() > 0) {
    last_max_area = v[0].get_area();
    last_idx = 0;
  } else if (v.size() == 1) {
    return 0;
  }

  for (int ii=1; ii<v.size(); ++ii) {
    if (v[ii].get_area() > last_max_area) {
      last_idx = ii;
      last_max_area = v[ii].get_area();
    }
  }

  return last_idx;
}


int find_closest(const std::vector<PersonRect> &v)
{
  int last_idx = -1;
  int last_min = -1;

  if (v.size() > 0) {
    last_min = v[0].get_dist();
    last_idx = 0;
  } else if (v.size() == 1) {
    return 0;
  }

  for (int ii=1; ii<v.size(); ++ii) {
    if (v[ii].get_dist() < last_min) {
      last_idx = ii;
      last_min = v[ii].get_dist();
    }
  }

  return last_idx;
}
