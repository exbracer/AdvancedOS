/*************************************************************************
	> File Name: lec04_p57.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月14日 13時23分59秒
 ************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/percpu.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/smp.h>

#define PRINT_PERF "[PERCPU]: "

struct task_struct *thread1, *thread2, *thread3;
void *my_var2;

static int thread_function(void * data)
{
    while(!kthread_should_stop())
    {
        int *local_ptr, cpu;
        local_ptr = get_cpu_ptr(my_var2);

        cpu = smp_processor_id();
        (*local_ptr)++;
        printk("cpu[%d] = %d\n", cpu, *local_ptr);
        put_cpu_var(my_var2);
        msleep(500);
    }
    do_exit(0);
}

static int __init my_mod_init(void)
{
    int *local_ptr;
    int cpu;

    printk(PRINT_PERF "Entering module.\n");

    my_var2 = alloc_percpu(int);
    if(!my_var2)
    {
        return -1;
    }

    for (cpu = 0; cpu < NR_CPUS; cpu ++)
    {
        local_ptr = per_cpu_ptr(my_var2, cpu);
        *local_ptr = 0;
        put_cpu();
    }

    wmb();

    thread1 = kthread_run(thread_function, NULL, "percpu-thread1");
    thread2 = kthread_run(thread_function, NULL, "percpu-thread2");
    thread3 = kthread_run(thread_function, NULL, "percpu-thread3");

    return 0;
}

static void __exit my_mod_exit(void)
{
    kthread_stop(thread1);
    kthread_stop(thread2);
    kthread_stop(thread3);
    
    free_percpu(my_var2);

    printk(KERN_INFO "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
