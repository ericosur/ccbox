#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <unistd.h>

using namespace std;

void issue_man_alert_v2(int dist)
{
  static int last_dist = 0;
  static int last_vol = 0;
  //static int last_epoch = 0;

  const int low_vol = 25;
  const int mid_vol = 50;
  const int high_vol = 95;
  const int max_vol = 100;
  const int threshold = 35;
  const int pt1 = 150;
  const int pt2 = 250;
  const int pt3 = 350;
  int ret_vol = 0;

  // will skip if dist <= 0
  if (dist <= 0) {
    return ;
  }

  if (last_dist == 0) {
    last_dist = dist;
  }

  // take it as no change
  if (abs(last_dist - dist) < threshold) {
    printf("dbg: within threshold,%d vs %d\n", last_dist, dist);
    return;
  }

  if (dist < pt1) {
    ret_vol = low_vol;
  } else if ( dist >= pt1 && dist < pt2 ) {
    ret_vol = mid_vol;
  } else if ( dist >= pt2 && dist < pt3 ) {
    ret_vol = high_vol;
  } else if (dist > pt3) {
    ret_vol = max_vol;
  }

  if (ret_vol != 0) {
    printf("issue_man_alert_v2: put %d\n", ret_vol);
  }

  if (abs(last_vol-ret_vol) < 10) {
    printf("no change %d vs %d\n", last_vol, ret_vol);
  }
  last_vol = ret_vol;
}

int get_num(int base)
{
    int max = 525;
    int min = 25;

    int step = std::rand() % 40;
    int direction = std::rand() % 3;

    int ret = 0;
    if (direction == 0) {
        ret = base + step;
    } else {
        ret = base - step;
    }
    if (ret > max)
        ret = max;
    if (ret < min)
        ret = min;

    return ret;
}

int main()
{
    std::srand(std::time(nullptr)); // use current time as seed for random generator

    const int repeat = 100;
    int base = 25 + std::rand() % 400;
    for (int i = 0 ; i < repeat; ++i) {
        int d = get_num(base);
        cout << d << "=====>";
        issue_man_alert_v2(d);
        sleep(1);
    }


    return 0;
}