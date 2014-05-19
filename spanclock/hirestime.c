#include "hirestime.h"

#if defined _WIN32 // Both 32 & 64 bit windows (cl.exe) define this.

int hirestime_freq(hires_freq *f){
    (void)QueryPerformanceFrequency(f);
    return 0;
}

hires_ctr * hirestime_read(hires_ctr *ctr) {
    (void)QueryPerformanceCounter(ctr);
    return ctr;
}

hires_ctr * hirestime_accum(hires_ctr *ctr) {

    hires_ctr cur;
    (void)QueryPerformanceCounter(&cur);
    ctr->QuadPart = cur.QuadPart - ctr->QuadPart;
    return ctr;
}

// result = a - b
hires_ctr hirestime_sub(hires_ctr a, hires_ctr b){

    hires_ctr r;
    r.QuadPart = a.QuadPart - b.QuadPart;
    return r;
}

hires_ctr hirestime_add(hires_ctr a, hires_ctr b){
    hires_ctr r;
    r.QuadPart = a.QuadPart + b.QuadPart;
    return r;
}

int hirestime_cmp0(hires_ctr val){

    if (val.QuadPart < 0)
        return -1;
    if (val.QuadPart > 0)
        return 1;
    return 0;
}

int hirestime_cmp(hires_ctr a, hires_ctr b){

    if (a.QuadPart > b.QuadPart)
        return 1;
    if (b.QuadPart > a.QuadPart)
        return -1;
    return 0;
}

void hirestime_usec_set(hires_ctr *val, long long usec, hires_freq const *f){
    val->QuadPart = (f->QuadPart * usec) / 1000000;
}

void hirestime_dset_sec(hires_ctr *val, double sec, hires_freq const *f){
    val->QuadPart = (f->QuadPart * sec);
}

double hirestime_seconds(hires_ctr ctr, hires_freq const *f){
    return (double)ctr.QuadPart / f->QuadPart;
}

double hirestime_usec(hires_ctr ctr, hires_freq const *f){
    return ((double)ctr.QuadPart * 1000000) / f->QuadPart;
}

#elif defined HAVE_CLOCK_MONOTONIC

// From:http://pubs.opengroup.org/onlinepubs/9699919799/functions/clock_getres.html
//
//    The clock_getres(), clock_gettime(), and clock_settime() functions shall fail if:
//
//    [EINVAL]
//    The clock_id argument does not specify a known clock.
//    The clock_gettime() function shall fail if:
//
//    [EOVERFLOW]
//    The number of seconds will not fit in an object of type time_t.
//
// - We check at configure time that we can call clock_getres(CLOCK_MONOTONIC).
// - We ASSUME that the inteval measured is < 100 years. (most certainly safe).
// - We assume that cross builds verify clock monotonic out of band or disable it
//   manualy.
// - We don't use clock_settime
//
// GIVEN all of the above we choose to IGNORE return codes from clock_getres and
// clock_gettime
//
// But, to help cross implementations, hirestime_freq returns the integer
// return value of clock_getres.
//
//
//
int hirestime_freq(hires_freq *f){
    return clock_getres(CLOCK_MONOTONIC, f);
}
hires_ctr * hirestime_read(hires_ctr *ctr) {
    clock_gettime(CLOCK_MONOTONIC, ctr);
    return ctr;
}

int hirestime_cmp0(hires_ctr val){

    if (val.tv_sec < 0)
        return -1;
    if (val.tv_sec > 0)
        return 1;
    if (val.tv_nsec < 0)
        return -1;
    if (val.tv_nsec > 0)
        return 1;
    return 0;
}

// result = a - b
hires_ctr hirestime_sub(hires_ctr a, hires_ctr b){

    hires_ctr result;
    if ( (a.tv_nsec - b.tv_nsec) < 0 ){
        result.tv_sec = a.tv_sec - b.tv_sec - 1;
        result.tv_nsec = 1000000000 + a.tv_nsec - b.tv_nsec;
    } else {
        result.tv_sec = a.tv_sec - b.tv_sec;
        result.tv_nsec = a.tv_nsec - b.tv_nsec;
    }
    return result;
}

hires_ctr hirestime_add(hires_ctr a, hires_ctr b){
    hires_ctr result;
    result.tv_sec = a.tv_sec + b.tv_sec;
    result.tv_nsec = a.tv_nsec + b.tv_nsec;
    if (result.tv_nsec >= 1000000000L) {
        result.tv_sec ++;
        result.tv_nsec = result.tv_nsec - 1000000000L;
    }
    return result;
}

int hirestime_cmp(hires_ctr a, hires_ctr b) {
    if (a.tv_sec < b.tv_sec)
        return -1;
    else if (a.tv_sec > b.tv_sec)
        return 1;
    else if (a.tv_nsec < b.tv_nsec)
        return -1;
    else if (a.tv_nsec > b.tv_nsec)
        return 1;
    else
        return 0;
}

double hirestime_seconds(hires_ctr ctr, hires_freq const *f){
    return (double)ctr.tv_sec + ((double)ctr.tv_nsec / 1000000000);
}

double hirestime_usec(hires_ctr ctr, hires_freq const *f) {
    return (double)ctr.tv_sec * 1000000 + (double)ctr.tv_nsec / 1000;
}

void hirestime_usec_set(hires_ctr *val, long long usec, hires_freq const *f) {
    val->tv_sec = usec / 1000000L;
    val->tv_nsec = (usec - (val->tv_sec * 1000000L)) * 1000L;
}

void hirestime_dset_sec(hires_ctr *val, double sec, hires_freq const *f){

    val->tv_sec = (time_t)sec;
    val->tv_nsec = (long) ((sec - (double) val->tv_sec) * 1000000000.0);
    if (sec < 0)
        val->tv_nsec *= -1.0;
}

#elif defined HAVE_GETTIMEOFDAY
#endif

#if ! defined _WIN32
hires_ctr *hirestime_accum(hires_ctr *ctr) {

    hires_ctr cur;
    hirestime_read(&cur);
    *ctr = hirestime_sub(cur, *ctr);
    return ctr;
}
#endif // #if ! defined _WIN32

hires_ctr hirestime_diffnow(hires_ctr prev){
    hires_ctr now;
    hirestime_read(&now);
    return hirestime_sub(now, prev);
}

// if a is greater, return 1, if b is greater -1, if eq return 0
int hirestime_diffcmp0(hires_ctr a, hires_ctr b){
    return hirestime_cmp0(hirestime_sub(a, b));
}

// return the minimum of a, b.
hires_ctr hirestime_mincopy(hires_ctr a, hires_ctr b){

    int cmp = hirestime_cmp(a, b);
    if (cmp > 0) // a is greater, so return b
        return b;

    return a;
}

