#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#define DEFAULT_WIDTH    424
#define DEFAULT_HEIGHT   240

#define ISNAN(X) ((X) != (X))

#ifndef MIN_DEPTH
// unit: mm
#define MIN_DEPTH           (141)
// unit: m
#define MIN_DEPTH_METER     (0.141)
#endif

#ifndef MAX_DEPTH
// unit: mm
#define MAX_DEPTH           (999)
// unit: m
#define MAX_DEPTH_METER     (0.999)
#endif

#ifndef DEPTH_SMALL_DIFF
#define DEPTH_SMALL_DIFF    (20)
#endif

#ifndef DEPTH_DIFF_TOO_LARGE
// unit: mm
#define DEPTH_DIFF_TOO_LARGE    (450)
#endif

#ifndef INVALID_DEGREE
#define INVALID_DEGREE      (999)
#endif

// abs(deg) < degree limit
#ifndef MAX_DEGREE_LIMIT
#define MAX_DEGREE_LIMIT    (45.0)
#endif

extern bool g_verbose;

#endif  // __COMMON_DEF_H__
