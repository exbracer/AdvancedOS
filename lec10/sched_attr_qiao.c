#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syscall.h>
#include <sched.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <asm-generic/int-l64.h>

// added by qiao
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

// added by qiao 
// for periodic tasks
#define NSEC_PER_SEC 1000000000ULL
static inline void timespec_add_us(struct timespec *t, uint64_t p)
{
    p *= 1000;
    p += t->tv_nsec;
    while (p >= NSEC_PER_SEC)
    {
        p -= NSEC_PER_SEC;
        t->tv_sec += 1;
    }
    t->tv_nsec = p;
}

static void wait_next_action(struct timespec *t, uint64_t period)
{
    // timespec_add_us(t, period);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, t, NULL);
    timespec_add_us(t, period);
}

static void do_something(struct timespec *s, int id)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    unsigned long long ns = (now.tv_sec - s->tv_sec)*NSEC_PER_SEC + (now.tv_nsec - s->tv_nsec);
    double ms = (double)ns/1000000;
    if (id == 0)
    {
        printf("A process: priority 99, period 1 ms, timestamp %lf ms\n", ms);
    }
    else if (id == 1)
    {
        printf("B process: priority 50, period 5 ms, timestamp %lf ms\n", ms);
    }
    else if (id == 2)
    {
        printf("C process: priority 10, period 100 ms, timestamp %lf ms\n", ms);
    }
}

/**
 * @struct sched_attr
 *
 * The sched_attr structure has scheduling policy and attirbutes.
 */
struct sched_attr {
	/**
	 * Size of this structure.
	 */
	__u32 size;

	/**
	 * Policy (SCHED_*).
	 */
	__u32 sched_policy;

	/**
	 * Flags.
	 */
	__u64 sched_flags;

	/**
	 * Nice value (SCHED_NORMAL, SCHED_BATCH).
	 */
	__s32 sched_nice;

	/**
	 * Static priority (SCHED_FIFO, SCHED_RR).
	 */
	__u32 sched_priority;

	/* Remaining fields are for SCHED_DEADLINE. */
	/**
	 * Running time [ns].
	 */
	__u64 sched_runtime;

	/**
	 * Deadline [ns].
	 */
	__u64 sched_deadline;

	/**
	 * Period [ns].
	 */
	__u64 sched_period;
};




static inline int sched_setattr(pid_t pid,
                                const struct sched_attr *attr,
                                unsigned int flags)
{
  return syscall(SYS_sched_setattr, pid, attr, flags);
}

static inline int sched_getattr(pid_t pid,
                                struct sched_attr *attr,
                                unsigned int size,
                                unsigned int flags)
{
  return syscall(SYS_sched_getattr, pid, attr, size, flags);
}

static inline pid_t gettid(void)
{
  return syscall(SYS_gettid);
}

static inline int64_t nr_online_cpus(void)
{
  return sysconf(_SC_NPROCESSORS_ONLN);
}

int migrate_thread_to_cpu(pid_t tid, int target_cpu)
{
  cpu_set_t *cpu_set;
  size_t size;
  int nr_cpus;
  int ret;

  if ((nr_cpus = nr_online_cpus()) == -1) {
    perror("nr_online_cpus()");
    return 1;
  }
		
  if (target_cpu < 0 || target_cpu >= nr_cpus) {
    fprintf(stderr, "Error: target_cpu %d is in [0,%d)\n", target_cpu, nr_cpus);
    return 2;
  }		
		
  if (!(cpu_set = CPU_ALLOC(nr_cpus))) {
    perror("CPU_ALLOC");
    return 3;
  }
  size = CPU_ALLOC_SIZE(nr_cpus);
  CPU_ZERO_S(size, cpu_set);
  CPU_SET_S(target_cpu, size, cpu_set);

  ret = sched_setaffinity(tid, size, cpu_set);
  
  CPU_FREE(cpu_set);
		
  return ret;
}

int main(int argc, char *argv[])
{
    pid_t pid1 = -42; // for process B
    pid_t pid2 = -42; // for process C
    int wstatus1 = -42;
    int wstatus2 = -42;

    int ret = -1;
    int id = 0;


    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    timespec_add_us(&start, 3000000);
    struct timespec timer;
    timer.tv_sec = start.tv_sec;
    timer.tv_nsec = start.tv_nsec;

    pid1 = fork();
    if (pid1 == 0)
    {
        id = 1;
    }
    else if (pid1 != -1)
    {
        pid2 = fork();
        if (pid2 == 0)
        {
            id = 2;
        }
    }

    // printf("%d, %d, %d\n", id, pid1, pid2);
    struct sched_attr attr;
    attr.size = sizeof(struct sched_attr);
    attr.sched_flags = 0;
    attr.sched_nice = 0;

    int res = -1;
    int count = 0;
    int max_count = 0;
    int period = 0;
    attr.sched_policy = SCHED_FIFO;
    if (id == 0)
    {
        attr.sched_priority = 99;
        period = 1000;
        max_count = 500;
    }
    else if (id == 1)
    {
        attr.sched_priority = 50;
        period = 5000;
        max_count = 100;
    }
    else if (id == 2)
    {
        attr.sched_priority = 10;
        period = 100000;
        max_count = 5;
    }
    
    if ((res = sched_setattr(gettid(), &attr, 0)) != 0)
    {
        perror("sched_setattr");
        exit(res);
    }
    migrate_thread_to_cpu(gettid(), 0);

    while (count < max_count)
    {
        wait_next_action(&timer, period);
        count ++;
        do_something(&start, id);
    }


    // join the process
    if (pid1 >= 0)
    {
        ret = waitpid(pid1, &wstatus1, 0);
    }
    if (pid2 >= 0)
    {
        ret = waitpid(pid2, &wstatus2, 0);
    }

    return 0;
}
