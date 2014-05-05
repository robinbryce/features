#include <stdio.h>
#include "hirestime.h"

typedef struct _hires_info {
    hires_ctr precision;
} hires_info;

// *carefully* measure a "subject" activity that takes < max microseconds.
// max: defaults to 1000000 microseconds
//
// derived from:
//  http://hg.python.org/peps/file/tip/pep-0418/clock_resolution.py
//
int hirestime_measure(hires_ctr *result, hires_ctr const *max,
        void (*activity)(void *), void *activity_ctx,
        int *measurements, int *cycles){

    int points = 0;
    int got_first_measurement = 0;
    hires_ctr ctr_default_max = {0};
    hires_ctr ctr_tmp, ctr_diff, ctr_end, ctr_previous, ctr_min;
    hires_freq f;

    if (measurements)
        *measurements = 0;
    if (cycles)
        *cycles = 0;

    // initialise our timeout.
    hirestime_freq(&f);
    if (!max){
        hirestime_set_usec(&ctr_default_max, 1000000, &f);
        max = &ctr_default_max;
    }

    // initialise our starting point for "course" timers.
    hirestime_read(&ctr_previous);
    hirestime_add(&ctr_end, max, &ctr_previous);

    for (;;){

        int i;

        // test the time out.
        if (hirestime_diffcmp0(&ctr_end, hirestime_read(&ctr_tmp)) <=0)
            goto measurement_end;
        if (points >= 3)
            // Once we manage 3 successful measurements, we are done.
            goto measurement_end;

        // ok, make up to 10 attempts to time the activity

        for (i=0; i < 10; i++){

            hirestime_read(&ctr_tmp);
            if (activity)
                activity(activity_ctx);
            hirestime_diffnow(&ctr_diff, &ctr_tmp);

            if (measurements)
                (*measurements) += 1;

            if (hirestime_cmp0(&ctr_diff) > 0)
                goto measurement_made;
        }

        hirestime_diffnow(&ctr_diff, &ctr_previous);
        if (hirestime_cmp0(&ctr_diff) <= 0){

            if (cycles) (*cycles) += 1;

            // note, we don't update ctr_previous ...
            continue;
        }

measurement_made:
        if (got_first_measurement){
            hirestime_mincopy(&ctr_min, &ctr_min, &ctr_diff);
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
}

void sleeper(void *ctx){
    Sleep((DWORD)ctx);
}

int main(int argc, char**argv){
    int status;
    int measurements, cycles;
    DWORD sleep_ms;
    hires_freq f;
    hires_ctr max;
    hires_ctr activity;
    hires_ctr precision;

    printf("running\n");
    hirestime_freq(&f);
    hirestime_dset_sec(&max, 5.0, &f);

    //measure the quickest 'sleep'. note that sleep may be interupted before
    //the specified wait period due to interupts etc.
    sleep_ms = 10;
    status = hirestime_measure(&activity, &max,
            sleeper, (void*)sleep_ms /*  NOT address of */,
            &measurements, &cycles);
    if (0 == status)
        printf("%f sec [m %d, c %d] - shortest sleep(%d)\n",
                hirestime_seconds(&activity, &f), sleep_ms,
                measurements, cycles
                );
    status = hirestime_measure(&precision, &max, (void*)0, (void*)0,
            &measurements, &cycles);
    if (0 == status)
        printf("[m %d, c %d]\n", measurements, cycles);
        printf("%d counts - precision\n", precision.QuadPart);
        printf("%f usec - precision\n", hirestime_usec(&precision, &f));

    printf("OK\n");
    return 0;
}
