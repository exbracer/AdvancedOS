#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <syscall.h>


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
  struct sched_param param;
  int policy;
  int ret;

  if (argc < 3) {
    fprintf(stderr, "usage: %s rr/fifo [priority]\n", argv[0]);
    exit(1);
  }
    
  if (strcasecmp(argv[1], "rr") == 0) {
    policy = SCHED_RR;
  } else if (strcasecmp(argv[1], "fifo") == 0) {
    policy = SCHED_FIFO;
  } else {
    fprintf(stderr, "Unknown Scheduling Policy %s", argv[1]);
    exit(2);
  }
  
  param.sched_priority = atoi(argv[2]);
  if ((ret = sched_setscheduler(0, policy, &param)) != 0) {
    perror("sched_setscheduler");
    return ret;
  }

  migrate_thread_to_cpu(gettid(), 0);

  while (1) {
    ;
  }
    

  return 0;
}
