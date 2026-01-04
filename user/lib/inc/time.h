#ifndef LAMBDA_LIB_TIME_H
#define LAMBDA_LIB_TIME_H

#include <stdint.h>

typedef int64_t time_t;

struct timespec {
    /** Time in seconds */
    time_t  tv_sec;
    /** Offset in nanoseconds */
    int32_t tv_nsec;
};

#endif
