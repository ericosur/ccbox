#ifdef USE_TESTDIST

#include <stdio.h>
#include <stdint.h>
#include <string>
//#include <nlohmann/json.hpp>

#include <pbox/pbox.h>

bool read_raw_from_bin(const char* fn, uint16_t* buffer, int w, int h)
{
    FILE* fp = fopen(fn, "rb");
    if (fp == NULL) {
        fprintf(stderr, "cannot open %s\n", fn);
        return false;
    }
    size_t ret = fread(buffer, sizeof(uint16_t), w*h, fp);
    //printf("ret = %d, %d\n", (int)ret, w*h);
    (void)ret;
    return true;
}

float get_dist_from_raw(uint16_t* buffer, int w, int h, int x, int y)
{
    int depth = 0;
    for (int j = 0 ; j < h; ++j) {
        for (int i = 0 ; i < w; ++i) {
            depth = *buffer++;
            if (i == x && j == y)
                break;
        }
    }

    float dist = depth / 1000.0;
    printf("get_dist_from_raw: %d, %.2f\n", depth, dist);
    return dist;
}

#endif  // USE_TESTDIST
