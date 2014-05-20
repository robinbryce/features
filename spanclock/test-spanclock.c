#include <stdio.h>
#include <assert.h>
#include "spanclock.h"

#if defined SPANCLOCK_CLOCK_MONOTONIC
#elif defined SPANCLOCK_GETTIMEOFDAY
#elif defined SPANCLOCK_QUERYPERFORMANCECOUNTER
#endif

void print_ctr(spanc_val ctr){
#if defined SPANCLOCK_VAL_TIMESPEC
    printf("tv_sec = %ld, tv_nsec = %ld", ctr.tv_sec, ctr.tv_nsec);
#elif defined SPANCLOCK_VAL_TIMEVAL
    printf("tv_sec = %ld, tv_usec = %ld", ctr.tv_sec, ctr.tv_usec);
#elif defined SPANCLOCK_QUERYPERFORMANCECOUNTER
    printf("counts = %d", ctr.QuadPart);
#endif
}
void print_scaled(spanc_val ctr) {
    double sec = spanclock_seconds(ctr);
    if (sec >= 1e-3)
        printf ("%.0f ms", sec * 1e3);
    else if (sec >= 1e-6)
        printf ("%.0f us", sec * 1e6);
    else
        printf ("%.0f ns", sec * 1e9);
}

int test_set(void){

    spanc_val v1, v2, v3;

    printf("[--------------------------------------]\n");
    printf("[test_set]\n");

    printf("_usec_set 1000000L (1.000)\n");
    spanclock_usec_set(&v1, 1000000L);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", spanclock_seconds(v1));

    printf("_usec_set 1001000L (1.001)\n");
    spanclock_usec_set(&v1, 1001000L);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", spanclock_seconds(v1));

    printf("_usec_set 1012000L (1.012)\n");
    spanclock_usec_set(&v1, 1012000L);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", spanclock_seconds(v1));

    printf("_usec_set 500000L (0.5)\n");
    spanclock_usec_set(&v1, 500000L);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", spanclock_seconds(v1));


    printf("_dset_sec 1.0\n");
    spanclock_dset_sec(&v1, 1.0);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", spanclock_seconds(v1));

    printf("_dset_sec 1.001\n");
    spanclock_dset_sec(&v1, 1.001);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", spanclock_seconds(v1));

    printf("_dset_sec 1.012\n");
    spanclock_dset_sec(&v1, 1.012);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", spanclock_seconds(v1));

    printf("_dset_sec 0.5\n");
    spanclock_dset_sec(&v1, 0.5);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", spanclock_seconds(v1));

    return 0;
}

int test_diffcmp_add_sub(){

    spanc_val a, b, b2;

    printf("[--------------------------------------]\n");
    printf("[test_diffcmp_add]\n");

    spanclock_usec_set(&a, 500000L);
    spanclock_usec_set(&b, 500000L);
    spanclock_usec_set(&b2, 500001L);

    printf("("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf("_cmp=%d, _diffcmp0=%d\n", spanclock_cmp(a, b), spanclock_diffcmp0(a, b));

    printf("("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf("_cmp=%d, _diffcmp0=%d\n", spanclock_cmp(a, b2), spanclock_diffcmp0(a, b2));

    printf("("); print_ctr(b2); printf("), ("); print_ctr(a); printf(")\n");
    printf("_cmp=%d, _diffcmp0=%d\n", spanclock_cmp(b2, a), spanclock_diffcmp0(b2, a));

    printf("\n");

    spanclock_usec_set(&a, 500000L);
    spanclock_usec_set(&b, 500000L);
    spanclock_usec_set(&b2, 500001L);

    printf("("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf("add -> ("); print_ctr(spanclock_add(a, b)); printf(")\n");
    printf("sub -> ("); print_ctr(spanclock_sub(a, b)); printf(")\n");

    printf("("); print_ctr(b); printf("), ("); print_ctr(a); printf(")\n");
    printf("add -> ("); print_ctr(spanclock_add(b, a)); printf(")\n");
    printf("sub -> ("); print_ctr(spanclock_sub(b, a)); printf(")\n");

    printf("("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf("add -> ("); print_ctr(spanclock_add(a, b2)); printf(")\n");
    printf("sub -> ("); print_ctr(spanclock_sub(a, b2)); printf(")\n");

    printf("\n");

    spanclock_usec_set(&a, 1500000L);
    spanclock_usec_set(&b, 1500000L);
    spanclock_usec_set(&b2, 500001L);

    printf("("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf("add -> ("); print_ctr(spanclock_add(a, b)); printf(")\n");
    printf("sub -> ("); print_ctr(spanclock_sub(a, b)); printf(")\n");

    printf("("); print_ctr(b); printf("), ("); print_ctr(a); printf(")\n");
    printf("add -> ("); print_ctr(spanclock_add(b, a)); printf(")\n");
    printf("sub -> ("); print_ctr(spanclock_sub(b, a)); printf(")\n");

    printf("("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf("add -> ("); print_ctr(spanclock_add(a, b2)); printf(")\n");
    printf("sub -> ("); print_ctr(spanclock_sub(a, b2)); printf(")\n");

    return 0;
}

int test_mincopy(){
    spanc_val a, b, b2;

    printf("[--------------------------------------]\n");
    printf("[test_mincopy]\n");

    spanclock_usec_set(&a, 500000L);
    spanclock_usec_set(&b, 500000L);
    spanclock_usec_set(&b2, 500001L);

    printf("mincopy ("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf(" -> ("); print_ctr(spanclock_mincopy(a, b)); printf(")\n");

    printf("mincopy ("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf(" -> ("); print_ctr(spanclock_mincopy(a, b2)); printf(")\n");

    printf("mincopy ("); print_ctr(b2); printf("), ("); print_ctr(a); printf(")\n");
    printf(" -> ("); print_ctr(spanclock_mincopy(b2, a)); printf(")\n");

    spanclock_usec_set(&a, 1500000L);
    spanclock_usec_set(&b, 1500000L);
    spanclock_usec_set(&b2, 1500001L);

    printf("mincopy ("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf(" -> ("); print_ctr(spanclock_mincopy(a, b)); printf(")\n");

    printf("mincopy ("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf(" -> ("); print_ctr(spanclock_mincopy(a, b2)); printf(")\n");

    printf("mincopy ("); print_ctr(b2); printf("), ("); print_ctr(a); printf(")\n");
    printf(" -> ("); print_ctr(spanclock_mincopy(b2, a)); printf(")\n");

    return 0;
}

int test_seconds(){
    spanc_val a, b, b2;

#if SPANCLOCK_CLOCK_MONOTONIC
    a.tv_sec = 17;
    a.tv_nsec = 86156333;

#else
    spanclock_dset_sec(&a, 17 + 86156333 / 1000000000.0);
#endif
    printf("[--------------------------------------]\n");
    printf("spanclock_seconds(");print_ctr(a); printf("\n");
    printf(" -> %f\n", spanclock_seconds(a)); printf("\n");

    return 0;
}

// *carefully* measure a "subject" activity that takes < max microseconds.
// max: defaults to 1000000 microseconds
//
// derived from:
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

void sleeper(void *ctx){
# if defined _WIN32
    Sleep(((DWORD) *(int*)ctx));
# else
    struct timespec req, rem;
    long long msec = *((int*)ctx);
    req.tv_sec = msec / 1000;
    req.tv_nsec = (msec - (req.tv_sec * 1000L)) * 1000000L;
    nanosleep(&req, &rem);
# endif
}


int main(int argc, char**argv){
    int status;
    int measurements, cycles;
    unsigned int sleep_ms;
    spanc_val max;
    spanc_val activity;
    spanc_val precision;

    printf("running\n");

#if 1
    test_set();
    test_diffcmp_add_sub();
    test_mincopy();
    test_seconds();
#endif

    spanclock_dset_sec(&max, 16.0);

    //measure the quickest 'sleep'. note that sleep may be interupted before
    //the specified wait period due to interupts etc.
    sleep_ms = 23;

    printf("----------------------------------------\n");
    printf("spanclock_measure\n");
    status = spanclock_measure(&activity, &max,
            sleeper, (void*)&sleep_ms,
            &measurements, &cycles);

    if (0 == status) {
        printf("- "); print_scaled(activity);
        printf(": sleep(%d) [m %d, c %d]\n",
                sleep_ms, measurements, cycles
              );
    }
    status = spanclock_measure(&precision, &max, (void*)0, (void*)0,
            &measurements, &cycles);
    if (0 == status) {
        printf("- "); print_scaled(precision);
        printf(": precision [m %d, c %d]\n", measurements, cycles);
        printf("OK\n");
    } else
        printf("error %d\n", status);
    return 0;
}
