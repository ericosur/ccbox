#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* for tm_gmtoff and tm_zone */
#endif

#include <time.h>

namespace pbox {

/* Checking errors returned by system calls was omitted for the sake of readability. */
int get_timeepoch()
{
    time_t t = time(NULL);
    // will same as "date +%s"
    //printf("%d\n", (int)t);
    return (int)t;
}

/*
## use shell script to check epoch to and from....

#!/bin/bash

# get epoch from date
testdate=$(date +%s)
echo $testdate

# convert epoch to local readable date/time string
date -d @$testdate

*/

}   // namespace pbox
