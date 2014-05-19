#include "spanclock.h"

#if defined _WIN32 // Both 32 & 64 bit windows (cl.exe) define this.

int spanclock_freq(spanc_freq *f){
    (void)QueryPerformanceFrequency(f);
    return 0;
}

spanc_val * spanclock_read(spanc_val *ctr) {
    (void)QueryPerformanceCounter(ctr);
    return ctr;
}

spanc_val * spanclock_accum(spanc_val *ctr) {

    spanc_val cur;
    (void)QueryPerformanceCounter(&cur);
    ctr->QuadPart = cur.QuadPart - ctr->QuadPart;
    return ctr;
}

// result = a - b
spanc_val spanclock_sub(spanc_val a, spanc_val b){

    spanc_val r;
    r.QuadPart = a.QuadPart - b.QuadPart;
    return r;
}

spanc_val spanclock_add(spanc_val a, spanc_val b){
    spanc_val r;
    r.QuadPart = a.QuadPart + b.QuadPart;
    return r;
}

int spanclock_cmp0(spanc_val val){

    if (val.QuadPart < 0)
        return -1;
    if (val.QuadPart > 0)
        return 1;
    return 0;
}

int spanclock_cmp(spanc_val a, spanc_val b){

    if (a.QuadPart > b.QuadPart)
        return 1;
    if (b.QuadPart > a.QuadPart)
        return -1;
    return 0;
}

void spanclock_usec_set(spanc_val *val, long long usec, spanc_freq const *f){
    val->QuadPart = (f->QuadPart * usec) / 1000000;
}

void spanclock_dset_sec(spanc_val *val, double sec, spanc_freq const *f){
    val->QuadPart = (f->QuadPart * sec);
}

double spanclock_seconds(spanc_val ctr, spanc_freq const *f){
    return (double)ctr.QuadPart / f->QuadPart;
}

double spanclock_usec(spanc_val ctr, spanc_freq const *f){
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
// But, to help cross implementations, spanclock_freq returns the integer
// return value of clock_getres.
//
//
//
int spanclock_freq(spanc_freq *f){
    return clock_getres(CLOCK_MONOTONIC, f);
}
spanc_val * spanclock_read(spanc_val *ctr) {
    clock_gettime(CLOCK_MONOTONIC, ctr);
    return ctr;
}

int spanclock_cmp0(spanc_val val){

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
spanc_val spanclock_sub(spanc_val a, spanc_val b){

    spanc_val result;
    if ( (a.tv_nsec - b.tv_nsec) < 0 ){
        result.tv_sec = a.tv_sec - b.tv_sec - 1;
        result.tv_nsec = 1000000000 + a.tv_nsec - b.tv_nsec;
    } else {
        result.tv_sec = a.tv_sec - b.tv_sec;
        result.tv_nsec = a.tv_nsec - b.tv_nsec;
    }
    return result;
}

spanc_val spanclock_add(spanc_val a, spanc_val b){
    spanc_val result;
    result.tv_sec = a.tv_sec + b.tv_sec;
    result.tv_nsec = a.tv_nsec + b.tv_nsec;
    if (result.tv_nsec >= 1000000000L) {
        result.tv_sec ++;
        result.tv_nsec = result.tv_nsec - 1000000000L;
    }
    return result;
}

int spanclock_cmp(spanc_val a, spanc_val b) {
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

double spanclock_seconds(spanc_val ctr, spanc_freq const *f){
    return (double)ctr.tv_sec + ((double)ctr.tv_nsec / 1000000000);
}

double spanclock_usec(spanc_val ctr, spanc_freq const *f) {
    return (double)ctr.tv_sec * 1000000 + (double)ctr.tv_nsec / 1000;
}

void spanclock_usec_set(spanc_val *val, long long usec, spanc_freq const *f) {
    val->tv_sec = usec / 1000000L;
    val->tv_nsec = (usec - (val->tv_sec * 1000000L)) * 1000L;
}

void spanclock_dset_sec(spanc_val *val, double sec, spanc_freq const *f){

    val->tv_sec = (time_t)sec;
    val->tv_nsec = (long) ((sec - (double) val->tv_sec) * 1000000000.0);
    if (sec < 0)
        val->tv_nsec *= -1.0;
}

#elif defined HAVE_GETTIMEOFDAY
#endif

#if ! defined _WIN32
spanc_val *spanclock_accum(spanc_val *ctr) {

    spanc_val cur;
    spanclock_read(&cur);
    *ctr = spanclock_sub(cur, *ctr);
    return ctr;
}
#endif // #if ! defined _WIN32

spanc_val spanclock_diffnow(spanc_val prev){
    spanc_val now;
    spanclock_read(&now);
    return spanclock_sub(now, prev);
}

// if a is greater, return 1, if b is greater -1, if eq return 0
int spanclock_diffcmp0(spanc_val a, spanc_val b){
    return spanclock_cmp0(spanclock_sub(a, b));
}

// return the minimum of a, b.
spanc_val spanclock_mincopy(spanc_val a, spanc_val b){

    int cmp = spanclock_cmp(a, b);
    if (cmp > 0) // a is greater, so return b
        return b;

    return a;
}

