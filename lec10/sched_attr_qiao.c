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
  struct sched_attr attr;
  int ret;
  
  if (argc < 2) {
    fprintf(stderr, "Usage: %s rr/fifo/deadline [priority]\n", argv[0]);
    exit(1);
  }

  attr.size = sizeof(struct sched_attr);
  attr.sched_flags = 0;
  attr.sched_nice = 0;
  
  if (strcmp(argv[1], "rr") == 0) {
    attr.sched_policy = SCHED_RR;
  } else if (strcmp(argv[1], "fifo") == 0) {
    attr.sched_policy = SCHED_FIFO;
  } else if (strcmp(argv[1], "deadline") == 0) {
    attr.sched_policy = SCHED_DEADLINE;
  } else {
    fprintf(stderr, "Unknown scheduling policy %s\n", argv[1]);
    exit(2);
  }
  
  if (attr.sched_policy == SCHED_RR || attr.sched_policy == SCHED_FIFO) {
    if (argc < 3) {
      fprintf(stderr, "SCHED_RR/FIFO require priority in argv[2]\n");
      exit(3);
    }
    attr.sched_priority = atoi(argv[2]);
  } else {
    attr.sched_priority = 0;
    attr.sched_runtime = 1024;
    attr.sched_deadline = 1024;
    attr.sched_period = 0;
  }  

  if ((ret = sched_setattr(gettid(), &attr, 0)) != 0) {
    perror("sched_setattr");
    exit(ret);
  }

  /* migrate thread to CPU 0. */
  migrate_thread_to_cpu(gettid(), 0);

  /* do infinite loop. */
  while (1) {
    ;
  }
  
  return 0;
}
