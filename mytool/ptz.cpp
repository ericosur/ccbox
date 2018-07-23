#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* for tm_gmtoff and tm_zone */
#endif

#include <stdio.h>
#include <time.h>
#include <string>

namespace mytool {

/* Checking errors returned by system calls was omitted for the sake of readability. */
int get_timeepoch()
{
    time_t t = time(NULL);
    // will same as "date +%s"
    //printf("%d\n", (int)t);
    return (int)t;
}

std::string get_timestring()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[128];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%x %X", timeinfo);
    return std::string(buffer);
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

}   // namespace mytool
