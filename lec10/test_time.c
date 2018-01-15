/*************************************************************************
	> File Name: test_time.c
	> Author: 
	> Mail: 
	> Created Time: 2018年01月15日 23時45分21秒
 ************************************************************************/

#include <stdio.h>
#include <time.h>

#define NSEC_PER_SEC 1000000000ULL
int main()
{
    struct timespec time1, time2, now;
    clock_gettime(CLOCK_REALTIME, &time1);

    clock_gettime(CLOCK_REALTIME, &now);

    unsigned long long ns = (now.tv_sec - time1.tv_sec)*NSEC_PER_SEC+(now.tv_nsec-time1.tv_nsec);
    double ms = (double)ns/1000000;

    printf("A process: priority 99, period 1 ms, timestamp %lf ms\n", ms);
    clock_gettime(CLOCK_REALTIME, &time2);

    printf("ns: %ld\n", time2.tv_nsec-time1.tv_nsec);

    return 0;
}
