#include "config.h"

#if defined _WIN32 // Then we are windows 32 or 64 bit
#include <windows.h> // for LARGE_INTEGER

typedef LARGE_INTEGER hires_ctr;
typedef LARGE_INTEGER hires_freq;

#elif defined HAVE_CLOCK_MONOTONIC
  #error "todo"
#elif defined HAVE_GETTIMEOFDAY
#else
  #error "unsupported environment, no appropriate timing facilities - (neither CLOCK_MONOTONIC nor gettimeofday)"
#endif


int hirestime_freq(hires_freq *f);
hires_ctr * hirestime_read(hires_ctr *ctr);
hires_ctr * hirestime_accum(hires_ctr *ctr);

hires_ctr * hirestime_diffnow(hires_ctr *diff, hires_ctr const*prev);

hires_ctr * hirestime_sub(hires_ctr *result, hires_ctr const *a, hires_ctr const *b);
hires_ctr * hirestime_add(hires_ctr *result, hires_ctr const *a, hires_ctr const *b);

int hirestime_cmp(hires_ctr const*a, hires_ctr const*b);
hires_ctr *hirestime_mincopy(hires_ctr *result, hires_ctr const *a, hires_ctr const *b);

int hirestime_cmp0(hires_ctr const*val);
int hirestime_diffcmp0(hires_ctr const*a, hires_ctr const*b);

double hirestime_seconds(hires_ctr const *ctr, hires_freq const *f);
double hirestime_usec(hires_ctr const *ctr, hires_freq const *f);

void hirestime_set_usec(hires_ctr *val, long long usec, hires_freq const *f);
void hirestime_dset_sec(hires_ctr *val, double sec, hires_freq const *f);
