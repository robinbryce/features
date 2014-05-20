#include "spanclock.h"

#if defined _WIN32 // Both 32 & 64 bit windows (cl.exe) define this.

LONGLONG spanclock_freq(void){

    static LONGLONG freq = 0;
    if (freq == 0){

        LARGE_INTEGER tmp;
        // Succeedes on all supported windows platforms after windows xp
        (void)QueryPerformanceFrequency(&tmp);
        freq = tmp.QuadPart;
    }
    return freq;
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

void spanclock_usec_set(spanc_val *val, long long usec){
    val->QuadPart = (spanclock_freq() * usec) / 1000000;
}

void spanclock_dset_sec(spanc_val *val, double sec){
    val->QuadPart = (spanclock_freq() * sec);
}

double spanclock_seconds(spanc_val ctr){
    return (double)ctr.QuadPart / spanclock_freq();
}

double spanclock_usec(spanc_val ctr){
    return ((double)ctr.QuadPart * 1000000) / spanclock_freq();
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

double spanclock_seconds(spanc_val ctr){
    return (double)ctr.tv_sec + ((double)ctr.tv_nsec / 1000000000);
}

double spanclock_usec(spanc_val ctr) {
    return (double)ctr.tv_sec * 1000000 + (double)ctr.tv_nsec / 1000;
}

void spanclock_usec_set(spanc_val *val, long long usec) {
    val->tv_sec = usec / 1000000L;
    val->tv_nsec = (usec - (val->tv_sec * 1000000L)) * 1000L;
}

void spanclock_dset_sec(spanc_val *val, double sec){

    val->tv_sec = (time_t)sec;
    val->tv_nsec = (long) ((sec - (double) val->tv_sec) * 1000000000.0);
    if (sec < 0)
        val->tv_nsec *= -1.0;
}

#elif defined HAVE_GETTIMEOFDAY

spanc_val * spanclock_read(spanc_val *ctr) {
    (void)gettimeofday(ctr, 0);
    return ctr;
}

int spanclock_cmp0(spanc_val val){

    if (val.tv_sec < 0)
        return -1;
    if (val.tv_sec > 0)
        return 1;
    if (val.tv_usec < 0)
        return -1;
    if (val.tv_usec > 0)
        return 1;
    return 0;
}

// result = a - b
spanc_val spanclock_sub(spanc_val a, spanc_val b){

    spanc_val result;
    if ( (a.tv_usec - b.tv_usec) < 0 ){
        result.tv_sec = a.tv_sec - b.tv_sec - 1;
        result.tv_usec = 1000000 + a.tv_usec - b.tv_usec;
    } else {
        result.tv_sec = a.tv_sec - b.tv_sec;
        result.tv_usec = a.tv_usec - b.tv_usec;
    }
    return result;
}

spanc_val spanclock_add(spanc_val a, spanc_val b){

    spanc_val result;
    result.tv_sec = a.tv_sec + b.tv_sec;
    result.tv_usec = a.tv_usec + b.tv_usec;
    if (result.tv_usec >= 1000000L) {
        result.tv_sec ++;
        result.tv_usec = result.tv_usec - 1000000L;
    }
    return result;
}

int spanclock_cmp(spanc_val a, spanc_val b) {

    if (a.tv_sec < b.tv_sec)
        return -1;
    else if (a.tv_sec > b.tv_sec)
        return 1;
    else if (a.tv_usec < b.tv_usec)
        return -1;
    else if (a.tv_usec > b.tv_usec)
        return 1;
    else
        return 0;
}

double spanclock_seconds(spanc_val ctr){
    return (double)ctr.tv_sec + ((double)ctr.tv_usec / 1000000);
}

double spanclock_usec(spanc_val ctr) {
    return (double)ctr.tv_sec * 1000000 + (double)ctr.tv_usec;
}

void spanclock_usec_set(spanc_val *val, long long usec) {
    val->tv_sec = usec / 1000000L;
    val->tv_usec = usec - (val->tv_sec * 1000000L);
}

void spanclock_dset_sec(spanc_val *val, double sec){

    val->tv_sec = (time_t)sec;
    val->tv_usec = (suseconds_t) ((sec - (double) val->tv_sec) * 1000000.0);
    if (sec < 0)
        val->tv_usec *= -1.0;
}
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

// spanclock_measure
//
// *carefully* measure a "subject" activity that takes < max microseconds.
//
// max:
//  defaults to 1 second (represented as 1000000 micro seconds)
//
// actitity:
//  pointer to a function to time, pass NULL to measure the effective resolution
//  of the timer implementation.
//
// activity_ctx:
//  A single pointer value to provide some context to activity()
//
// See test-spanclock.c for example usage.
//
// derived from:-
//  http://hg.python.org/peps/file/tip/pep-0418/clock_resolution.py
//
int spanclock_measure(spanc_val *result, spanc_val const *max,
        void (*activity)(void *), void *activity_ctx,
        int *measurements, int *cycles){

    double a,b,c;
    spanc_val _max;
    int points = 0;
    int got_first_measurement = 0;
    spanc_val ctr_tmp, ctr_diff, ctr_end, ctr_previous, ctr_min;

    if (measurements)
        *measurements = 0;
    if (cycles)
        *cycles = 0;

    // initialise our timeout.
    if (!max)
        spanclock_usec_set(&_max, 2000000);
    else
        _max = *max;

    // initialise our starting point for "course" timers.
    spanclock_read(&ctr_previous);
    ctr_end = spanclock_add(_max, ctr_previous);

    a = spanclock_seconds(ctr_previous);
    b = spanclock_seconds(_max);
    c = spanclock_seconds(ctr_end);

    for (;;){

        int i;

        // test the time out.
        if (spanclock_diffcmp0(ctr_end, *spanclock_read(&ctr_tmp)) <=0)
            goto measurement_end;

        if (points >= 3)
            // Once we manage 3 successful measurements, we are done.
            goto measurement_end;

        // ok, make up to 10 attempts to time the activity

        for (i=0; i < 10; i++){

            spanclock_read(&ctr_tmp);
            if (activity)
                activity(activity_ctx);
            ctr_diff = spanclock_diffnow(ctr_tmp);

            if (measurements)
                (*measurements) += 1;

            if (spanclock_cmp0(ctr_diff) > 0)
                goto measurement_made;
        }

        // if the current time is still reading behind or the same as the
        // "previous" reading, keep going without updating ctr_previous, in
        // this situation we are eventually going to find the (poor) precision
        // or hit our timeout and fail.
        ctr_diff = spanclock_diffnow(ctr_previous);
        if (spanclock_cmp0(ctr_diff) <= 0){

            if (cycles) (*cycles) += 1;

            // note, we don't update ctr_previous ...
            continue;
        }

measurement_made:
        if (got_first_measurement){
            ctr_min = spanclock_mincopy(ctr_min, ctr_diff);
        } else {
            ctr_min = ctr_diff;
        }
        points += 1;
        spanclock_read(&ctr_previous);

        if (cycles) (*cycles) += 1;
    }

measurement_end:

    if (points >= 3) {
        *result = ctr_min;
        return 0;
    }
    return -1;
}

//:END
