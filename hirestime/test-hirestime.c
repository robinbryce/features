#include <stdio.h>
#include "hirestime.h"

#if defined HIRESTIME_CLOCK_MONOTONIC
#elif defined HIRESTIME_GETTIMEOFDAY
#elif defined HIRESTIME_QUERYPERFORMANCECOUNTER
#endif

void print_ctr(hires_ctr ctr){
#if defined HIRESTIME_CLOCK_MONOTONIC
    printf("tv_sec = %ld, tv_nsec = %ld", ctr.tv_sec, ctr.tv_nsec);
#elif defined HIRESTIME_QUERYPERFORMANCECOUNTER
    printf("counts = %d", ctr.QuadPart);
#endif
}
void print_scaled(hires_ctr ctr, hires_freq f) {
    double sec = hirestime_seconds(ctr, &f);
    if (sec >= 1e-3)
        printf ("%.0f ms", sec * 1e3);
    else if (sec >= 1e-6)
        printf ("%.0f us", sec * 1e6);
    else
        printf ("%.0f ns", sec * 1e9);
}

int test_set(void){

    hires_freq f;
    hires_ctr v1, v2, v3;
    hirestime_freq(&f);

    printf("[--------------------------------------]\n");
    printf("[test_set]\n");

    printf("_usec_set 1000000L (1.000)\n");
    hirestime_usec_set(&v1, 1000000L, &f);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", hirestime_seconds(v1, &f));

    printf("_usec_set 1000001L (1.001)\n");
    hirestime_usec_set(&v1, 1000001L, &f);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", hirestime_seconds(v1, &f));

    printf("_usec_set 1000012L (1.012)\n");
    hirestime_usec_set(&v1, 1000012L, &f);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", hirestime_seconds(v1, &f));

    printf("_usec_set 500000L (0.5)\n");
    hirestime_usec_set(&v1, 500000L, &f);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", hirestime_seconds(v1, &f));


    printf("_dset_sec 1.0\n");
    hirestime_dset_sec(&v1, 1.0, &f);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", hirestime_seconds(v1, &f));

    printf("_dset_sec 1.001\n");
    hirestime_usec_set(&v1, 1.001, &f);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", hirestime_seconds(v1, &f));

    printf("_dset_sec 1.012\n");
    hirestime_dset_sec(&v1, 1.012, &f);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", hirestime_seconds(v1, &f));

    printf("_dset_sec 0.5\n");
    hirestime_dset_sec(&v1, 0.5, &f);
    print_ctr(v1); printf("\n");
    printf("_seconds() -> %f\n", hirestime_seconds(v1, &f));

    return 0;
}

int test_diffcmp_add_sub(){

    hires_freq f;
    hires_ctr a, b, b2;
    hirestime_freq(&f);

    printf("[--------------------------------------]\n");
    printf("[test_diffcmp_add]\n");

    hirestime_usec_set(&a, 500000L, &f);
    hirestime_usec_set(&b, 500000L, &f);
    hirestime_usec_set(&b2, 500001L, &f);

    printf("("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf("_cmp=%d, _diffcmp0=%d\n", hirestime_cmp(a, b), hirestime_diffcmp0(a, b));

    printf("("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf("_cmp=%d, _diffcmp0=%d\n", hirestime_cmp(a, b2), hirestime_diffcmp0(a, b2));

    printf("("); print_ctr(b2); printf("), ("); print_ctr(a); printf(")\n");
    printf("_cmp=%d, _diffcmp0=%d\n", hirestime_cmp(b2, a), hirestime_diffcmp0(b2, a));

    printf("\n");

    hirestime_usec_set(&a, 500000L, &f);
    hirestime_usec_set(&b, 500000L, &f);
    hirestime_usec_set(&b2, 500001L, &f);

    printf("("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf("add -> ("); print_ctr(hirestime_add(a, b)); printf(")\n");
    printf("sub -> ("); print_ctr(hirestime_sub(a, b)); printf(")\n");

    printf("("); print_ctr(b); printf("), ("); print_ctr(a); printf(")\n");
    printf("add -> ("); print_ctr(hirestime_add(b, a)); printf(")\n");
    printf("sub -> ("); print_ctr(hirestime_sub(b, a)); printf(")\n");

    printf("("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf("add -> ("); print_ctr(hirestime_add(a, b2)); printf(")\n");
    printf("sub -> ("); print_ctr(hirestime_sub(a, b2)); printf(")\n");

    printf("\n");

    hirestime_usec_set(&a, 1500000L, &f);
    hirestime_usec_set(&b, 1500000L, &f);
    hirestime_usec_set(&b2, 500001L, &f);

    printf("("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf("add -> ("); print_ctr(hirestime_add(a, b)); printf(")\n");
    printf("sub -> ("); print_ctr(hirestime_sub(a, b)); printf(")\n");

    printf("("); print_ctr(b); printf("), ("); print_ctr(a); printf(")\n");
    printf("add -> ("); print_ctr(hirestime_add(b, a)); printf(")\n");
    printf("sub -> ("); print_ctr(hirestime_sub(b, a)); printf(")\n");

    printf("("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf("add -> ("); print_ctr(hirestime_add(a, b2)); printf(")\n");
    printf("sub -> ("); print_ctr(hirestime_sub(a, b2)); printf(")\n");

    return 0;
}

int test_mincopy(){
    hires_freq f;
    hires_ctr a, b, b2;
    hirestime_freq(&f);

    printf("[--------------------------------------]\n");
    printf("[test_mincopy]\n");

    hirestime_usec_set(&a, 500000L, &f);
    hirestime_usec_set(&b, 500000L, &f);
    hirestime_usec_set(&b2, 500001L, &f);

    printf("mincopy ("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf(" -> ("); print_ctr(hirestime_mincopy(a, b)); printf(")\n");

    printf("mincopy ("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf(" -> ("); print_ctr(hirestime_mincopy(a, b2)); printf(")\n");

    printf("mincopy ("); print_ctr(b2); printf("), ("); print_ctr(a); printf(")\n");
    printf(" -> ("); print_ctr(hirestime_mincopy(b2, a)); printf(")\n");

    hirestime_usec_set(&a, 1500000L, &f);
    hirestime_usec_set(&b, 1500000L, &f);
    hirestime_usec_set(&b2, 1500001L, &f);

    printf("mincopy ("); print_ctr(a); printf("), ("); print_ctr(b); printf(")\n");
    printf(" -> ("); print_ctr(hirestime_mincopy(a, b)); printf(")\n");

    printf("mincopy ("); print_ctr(a); printf("), ("); print_ctr(b2); printf(")\n");
    printf(" -> ("); print_ctr(hirestime_mincopy(a, b2)); printf(")\n");

    printf("mincopy ("); print_ctr(b2); printf("), ("); print_ctr(a); printf(")\n");
    printf(" -> ("); print_ctr(hirestime_mincopy(b2, a)); printf(")\n");

    return 0;
}

int test_seconds(){
    hires_freq f;
    hires_ctr a, b, b2;
    hirestime_freq(&f);

#if HIRESTIME_CLOCK_MONOTONIC
    a.tv_sec = 17;
    a.tv_nsec = 86156333;

#else
    hirestime_dsec_set(17 + 86156333 / 1000000000.0);
#endif
    printf("[--------------------------------------]\n");
    printf("hirestime_seconds(");print_ctr(a); printf("\n");
    printf(" -> %f\n", hirestime_seconds(a, &f)); printf("\n");

    return 0;
}

// *carefully* measure a "subject" activity that takes < max microseconds.
// max: defaults to 1000000 microseconds
//
// derived from:
//  http://hg.python.org/peps/file/tip/pep-0418/clock_resolution.py
//
int hirestime_measure(hires_ctr *result, hires_ctr const *max,
        void (*activity)(void *), void *activity_ctx,
        int *measurements, int *cycles){

    double a,b,c;
    hires_ctr _max;
    int points = 0;
    int got_first_measurement = 0;
    hires_ctr ctr_tmp, ctr_diff, ctr_end, ctr_previous, ctr_min;
    hires_freq f;

    if (measurements)
        *measurements = 0;
    if (cycles)
        *cycles = 0;

    // initialise our timeout.
    hirestime_freq(&f);
    if (!max)
        hirestime_usec_set(&_max, 2000000, &f);
    else
        _max = *max;

    // initialise our starting point for "course" timers.
    hirestime_read(&ctr_previous);
    ctr_end = hirestime_add(_max, ctr_previous);

    a = hirestime_seconds(ctr_previous, &f);
    b = hirestime_seconds(_max, &f);
    c = hirestime_seconds(ctr_end, &f);

    for (;;){

        int i;

        // test the time out.
        if (hirestime_diffcmp0(ctr_end, *hirestime_read(&ctr_tmp)) <=0)
            goto measurement_end;

        if (points >= 3)
            // Once we manage 3 successful measurements, we are done.
            goto measurement_end;

        // ok, make up to 10 attempts to time the activity

        for (i=0; i < 10; i++){

            hirestime_read(&ctr_tmp);
            if (activity)
                activity(activity_ctx);
            ctr_diff = hirestime_diffnow(ctr_tmp);

            if (measurements)
                (*measurements) += 1;

            if (hirestime_cmp0(ctr_diff) > 0)
                goto measurement_made;
        }

        // if the current time is still reading behind or the same as the
        // "previous" reading, keep going without updating ctr_previous, in
        // this situation we are eventually going to find the (poor) precision
        // or hit our timeout and fail.
        ctr_diff = hirestime_diffnow(ctr_previous);
        if (hirestime_cmp0(ctr_diff) <= 0){

            if (cycles) (*cycles) += 1;

            // note, we don't update ctr_previous ...
            continue;
        }

measurement_made:
        if (got_first_measurement){
            ctr_min = hirestime_mincopy(ctr_min, ctr_diff);
        } else {
            ctr_min = ctr_diff;
        }
        points += 1;
        hirestime_read(&ctr_previous);

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
    hirestime_usec_set(&req, *((int*)ctx) * 1000, NULL);
    nanosleep(&req, &rem);
# endif
}


int main(int argc, char**argv){
    int status;
    int measurements, cycles;
    unsigned int sleep_ms;
    hires_freq f;
    hires_ctr max;
    hires_ctr activity;
    hires_ctr precision;

    printf("running\n");

#if 0
    test_set();
    test_diffcmp_add_sub();
    test_mincopy();
    test_seconds();
#endif

    hirestime_freq(&f);
    hirestime_dset_sec(&max, 16.0, &f);

    //measure the quickest 'sleep'. note that sleep may be interupted before
    //the specified wait period due to interupts etc.
    sleep_ms = 23;

    printf("----------------------------------------\n");
    printf("hirestime_measure\n");
    status = hirestime_measure(&activity, &max,
            sleeper, (void*)&sleep_ms,
            &measurements, &cycles);

    if (0 == status) {
        printf("- "); print_scaled(activity, f);
        printf(": sleep(%d) [m %d, c %d]\n",
                sleep_ms, measurements, cycles
              );
    }
    status = hirestime_measure(&precision, &max, (void*)0, (void*)0,
            &measurements, &cycles);
    if (0 == status) {
        printf("- "); print_scaled(precision, f);
        printf(": precision [m %d, c %d]\n", measurements, cycles);
        printf("OK\n");
    } else
        printf("error %d\n", status);
    return 0;
}
