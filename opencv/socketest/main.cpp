#include "readset.h"
#include "mytool/mytool.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;
using namespace cv;

uint8_t* read_image(const string fn, size_t* size)
{
    Mat tstMat = imread(fn);

    vector<uint8_t> inImage;
    imencode(".jpg", tstMat, inImage);
    size_t datalen = inImage.size();
    cout << "datalen: " << datalen << "\n";

    uint8_t* buffer = (uint8_t*)malloc(inImage.size());
    *size = inImage.size();
    std::copy(inImage.begin(), inImage.end(), buffer);
    //free(buffer);

    cout << "read into buffer: " << fn << endl;
    return buffer;
}

// refer to:
// http://beej-zhtw.netdpi.net/09-man-manual/9-13-inet_ntoa-inet_aton-inet_addr

void ljust(char* str, size_t filllen)
{
    size_t len = strlen(str);
    memset(&str[len], 0x20, filllen - len);
}

void test_transfer(const std::vector<std::string> files)
{
#ifndef USE_JSON
#error need read json settings
#endif

    ReadSetting* sett = ReadSetting::getInstance();

    int sockfd;
    struct sockaddr_in dest;
    int ret;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

    const int SIZEBUF_SIZE = 16;
    char size_buf[SIZEBUF_SIZE];
    const int RECVBUF_SIZE = 128;
    char recv_buf[RECVBUF_SIZE];
    bool bCouldSend = true;

    for (size_t ii = 0; ii < files.size(); ++ii) {

        if (bCouldSend) {
            size_t size;
            uint8_t* buffer = read_image(files[ii], &size);

            if (buffer == NULL) {
                printf("buffer is NULL\n");
                break;
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
            bCouldSend = false;
        } else {
            cout << "bypass: " << files[ii] << "\n";
        }

        bzero(recv_buf, RECVBUF_SIZE);
        ret = recv(sockfd, recv_buf, RECVBUF_SIZE, MSG_DONTWAIT);
        //printf("ret of recv: %d, %s\n", ret, recv_buf);
        if (ret == -1) {
            printf("wait a moment\n");
            usleep(500*1000);
        }
        if (strcmp(recv_buf, "ok") == 0) {
            bCouldSend = true;
        }

    }   // for loop

    close(sockfd);
}

int main()
{
    std::vector<std::string> v;
    v.push_back("../lena.jpg");
    v.push_back("../f0011.jpg");
    v.push_back("../f0023.jpg");
    v.push_back("../test.jpg");
    test_transfer(v);
    return 0;
}
