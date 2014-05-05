#include "hirestime.h"

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

    return 0;
}

// result = a - b
hires_ctr * hirestime_sub(hires_ctr *result, hires_ctr const *a, hires_ctr const *b){

    result->QuadPart = a->QuadPart - b->QuadPart;
    return result;
}
hires_ctr * hirestime_add(hires_ctr *result, hires_ctr const *a, hires_ctr const *b){
    result->QuadPart = a->QuadPart + b->QuadPart;
    return result;
}

hires_ctr * hirestime_diffnow(hires_ctr *diff, hires_ctr const*prev){
    hires_ctr now;
    hirestime_read(&now);
    hirestime_sub(diff, &now, prev);
    return diff;
}

int hirestime_cmp0(hires_ctr const*val){

    if (val->QuadPart < 0)
        return -1;
    if (val->QuadPart > 0)
        return 1;
    return 0;
}

int hirestime_cmp(hires_ctr const*a, hires_ctr const*b){

    if (a->QuadPart > b->QuadPart)
        return 1;
    if (b->QuadPart > a->QuadPart)
        return -1;
    return 0;
}

hires_ctr *hirestime_mincopy(hires_ctr *result, hires_ctr const *a, hires_ctr const *b){
    int cmp = hirestime_cmp(a, b);
    if (cmp > 0) // a is greater
        result->QuadPart = b->QuadPart;

    // in the case where a and b are equal we still have to copy, this makes
    // the case where b is greater the same as for when a=b
    result->QuadPart = a->QuadPart;

    return result;
}

// if a is greater, return 1, if b is greater -1, if eq return 0
int hirestime_diffcmp0(hires_ctr const*a, hires_ctr const*b){
    hires_ctr diff;
    hirestime_sub(&diff, a, b);
    return hirestime_cmp0(&diff);
}

void hirestime_set_usec(hires_ctr *val, long long usec, hires_freq const *f){
    val->QuadPart = (f->QuadPart * usec) / 1000000;
}
void hirestime_dset_sec(hires_ctr *val, double sec, hires_freq const *f){
    val->QuadPart = (f->QuadPart * sec);
}
double hirestime_seconds(hires_ctr const *ctr, hires_freq const *f){
    return (double)ctr->QuadPart / f->QuadPart;
}
double hirestime_usec(hires_ctr const *ctr, hires_freq const *f){
    return ((double)ctr->QuadPart * 1000000) / f->QuadPart;
}
