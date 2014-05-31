#include <stdio.h>
#include <assert.h>
#include "gtest/gtest.h"
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

TEST(spanclock, diffcmp_add_sub){

	spanc_val a, b, b2;

	spanclock_usec_set(&a, 500000L);
	spanclock_usec_set(&b, 500000L);
	spanclock_usec_set(&b2, 500001L);

	EXPECT_EQ(spanclock_cmp(a,b), 0);
	EXPECT_EQ(spanclock_diffcmp0(a,b), 0);

	EXPECT_EQ(spanclock_cmp(a, b2), -1);
	EXPECT_EQ(spanclock_diffcmp0(a, b2), -1);

	EXPECT_EQ(spanclock_cmp(b2, a), 1);
	EXPECT_EQ(spanclock_diffcmp0(b2, a), 1);

	EXPECT_FLOAT_EQ(1.0, spanclock_seconds(spanclock_add(a, b)));
	EXPECT_FLOAT_EQ(0.0, spanclock_seconds(spanclock_sub(a, b)));

	EXPECT_FLOAT_EQ(1.0, spanclock_seconds(spanclock_add(b, a)));
	EXPECT_FLOAT_EQ(0.0, spanclock_seconds(spanclock_sub(b, a)));

	EXPECT_FLOAT_EQ(1.000001, spanclock_seconds(spanclock_add(a, b2)));
	EXPECT_FLOAT_EQ(-0.000001, spanclock_seconds(spanclock_sub(a, b2)));

	spanclock_usec_set(&a, 1500000L);
	spanclock_usec_set(&b, 1500000L);
	spanclock_usec_set(&b2, 500001L);

	EXPECT_FLOAT_EQ(3.0,spanclock_seconds(spanclock_add(a, b)));
	EXPECT_FLOAT_EQ(0.0,spanclock_seconds(spanclock_sub(a, b)));

	EXPECT_FLOAT_EQ(3.0,spanclock_seconds(spanclock_add(b, a)));
	EXPECT_FLOAT_EQ(0.0,spanclock_seconds(spanclock_sub(b, a)));

	EXPECT_FLOAT_EQ(2.000001, spanclock_seconds(spanclock_add(a, b2)));
	EXPECT_FLOAT_EQ(0.999999, spanclock_seconds(spanclock_sub(a, b2)));

}

TEST(spanclock, mincopy){

	spanc_val a, b, b2;

	spanclock_usec_set(&a, 500000L);
	spanclock_usec_set(&b, 500000L);
	spanclock_usec_set(&b2, 500001L);

	EXPECT_EQ(spanclock_mincopy(a, b), a);
	EXPECT_EQ(spanclock_mincopy(a, b2), a);
	EXPECT_EQ(spanclock_mincopy(b2, a), a);

	spanclock_usec_set(&a, 1500000L);
	spanclock_usec_set(&b, 1500000L);
	spanclock_usec_set(&b2, 1500001L);

	EXPECT_EQ(spanclock_mincopy(a, b), a);
	EXPECT_EQ(spanclock_mincopy(a, b2), a);
	EXPECT_EQ(spanclock_mincopy(b2, a), a);
}

TEST(spanclock, seconds){

	spanc_val a, b, b2;

#if SPANCLOCK_CLOCK_MONOTONIC
	a.tv_sec = 17;
	a.tv_nsec = 86156333;

#else
	spanclock_dset_sec(&a, 17 + 86156333 / 1000000000.0);
#endif
	EXPECT_FLOAT_EQ(spanclock_seconds(a), 17 + 86156333 / 1000000000.0);
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

TEST(spanclock, measure){

	int status;
	int measurements, cycles;
	unsigned int sleep_ms;
	spanc_val max;
	spanc_val activity;
	spanc_val precision;

	spanclock_dset_sec(&max, 16.0);

	//measure the quickest 'sleep'. note that sleep may be interupted before
	//the specified wait period due to interupts etc.
	sleep_ms = 23;

	printf(" spanclock_measure with sleeper(ms=%d)\n", sleep_ms);

	status = spanclock_measure(&activity, &max,
		sleeper, (void*)&sleep_ms,
		&measurements, &cycles);

	EXPECT_EQ(status, 0);

	if (0 == status) {
		printf(" - "); print_scaled(activity);
		printf(" : sleep(%d) [m %d, c %d]\n",
				sleep_ms, measurements, cycles
			);
	}

	printf(" spanclock_measure finest granularity (precision)\n", sleep_ms);

	status = spanclock_measure(&precision, &max, ((void(*)(void*))0), (void*)0,
			&measurements, &cycles);

	EXPECT_EQ(status, 0);

	if (0 == status) {
		printf(" - "); print_scaled(precision);
		printf(" : precision [m %d, c %d]\n", measurements, cycles);
	}
}

int main(int argc, char**argv){

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
