#include "config.h"

#if defined _WIN32 // Then we are windows 32 or 64 bit
#include <windows.h> // for LARGE_INTEGER

#define SPANCLOCK_QUERYPERFORMANCECOUNTER (1)

typedef LARGE_INTEGER spanc_val;
typedef LARGE_INTEGER spanc_freq;

#elif defined HAVE_CLOCK_MONOTONIC

#define SPANCLOCK_CLOCK_MONOTONIC (1)

#include <time.h>

typedef struct timespec spanc_freq;
typedef struct timespec spanc_val;

#elif defined HAVE_GETTIMEOFDAY

#define SPANCLOCK_GETTIMEOFDAY (1)

#else
  #error "unsupported environment, no appropriate timing facilities - (neither CLOCK_MONOTONIC nor gettimeofday)"
#endif

// NOTE: frequency on posix platforms
//
// For platforms with HAVE_CLOCK_MONOTONIC or HAVE_GETTIMEOFDAY frequency is
// redundant. spanclock_freq will perform as expected but all other api's which
// accept a spanc_freq parameter ignore the value. Where the parameter is a
// pointer NULL is always safe.
int spanclock_freq(spanc_freq *f);

spanc_val * spanclock_read(spanc_val *ctr);
spanc_val * spanclock_accum(spanc_val *ctr);

spanc_val spanclock_sub(spanc_val a, spanc_val b);
spanc_val spanclock_add(spanc_val a, spanc_val b);

spanc_val spanclock_diffnow(spanc_val prev);

int spanclock_cmp(spanc_val a, spanc_val b);

int spanclock_cmp0(spanc_val val);
int spanclock_diffcmp0(spanc_val a, spanc_val b);

spanc_val spanclock_mincopy(spanc_val a, spanc_val b);

double spanclock_seconds(spanc_val ctr, spanc_freq const *f);
double spanclock_usec(spanc_val ctr, spanc_freq const *f);

void spanclock_usec_set(spanc_val *val, long long usec, spanc_freq const *f);
void spanclock_dset_sec(spanc_val *val, double sec, spanc_freq const *f);

