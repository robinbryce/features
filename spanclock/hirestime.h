#include "config.h"

#if defined _WIN32 // Then we are windows 32 or 64 bit
#include <windows.h> // for LARGE_INTEGER

#define HIRESTIME_QUERYPERFORMANCECOUNTER (1)

typedef LARGE_INTEGER hires_ctr;
typedef LARGE_INTEGER hires_freq;

#elif defined HAVE_CLOCK_MONOTONIC

#define HIRESTIME_CLOCK_MONOTONIC (1)

#include <time.h>

typedef struct timespec hires_freq;
typedef struct timespec hires_ctr;

#elif defined HAVE_GETTIMEOFDAY

#define HIRESTIME_GETTIMEOFDAY (1)

#else
  #error "unsupported environment, no appropriate timing facilities - (neither CLOCK_MONOTONIC nor gettimeofday)"
#endif

// NOTE: frequency on posix platforms
//
// For platforms with HAVE_CLOCK_MONOTONIC or HAVE_GETTIMEOFDAY frequency is
// redundant. hirestime_freq will perform as expected but all other api's which
// accept a hires_freq parameter ignore the value. Where the parameter is a
// pointer NULL is always safe.
int hirestime_freq(hires_freq *f);

hires_ctr * hirestime_read(hires_ctr *ctr);
hires_ctr * hirestime_accum(hires_ctr *ctr);

hires_ctr hirestime_sub(hires_ctr a, hires_ctr b);
hires_ctr hirestime_add(hires_ctr a, hires_ctr b);

hires_ctr hirestime_diffnow(hires_ctr prev);

int hirestime_cmp(hires_ctr a, hires_ctr b);

int hirestime_cmp0(hires_ctr val);
int hirestime_diffcmp0(hires_ctr a, hires_ctr b);

hires_ctr hirestime_mincopy(hires_ctr a, hires_ctr b);

double hirestime_seconds(hires_ctr ctr, hires_freq const *f);
double hirestime_usec(hires_ctr ctr, hires_freq const *f);

void hirestime_usec_set(hires_ctr *val, long long usec, hires_freq const *f);
void hirestime_dset_sec(hires_ctr *val, double sec, hires_freq const *f);

