/**
 **  simple opencv + realsense
 **/

#include "rsutil.h"

#include <iostream>
#include <iomanip>

auto window_name = "Display Image";


int main()
{
    if ( !rsutil::show_rsinfo() ) {
        printf("no Intel Realsense camera, exit...\n");
        exit(-1);
    }

    return 0;
}

