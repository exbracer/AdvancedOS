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
DEFINE_PER_CPU(int, my_var);

static int thread_function(void * data)
{
    while(!kthread_should_stop())
    {
        int cpu;
        get_cpu_var(my_var) ++;

        cpu = smp_processor_id();
        printk("cpu[%d] = %d\n", cpu, get_cpu_var(my_var));
        put_cpu_var(my_var);
        msleep(500);
    }
}

static int __init my_mod_init(void)
{
    int cpu;

    printk(PRINT_PERF "Entering module.\n");

    for (cpu = 0; cpu < NR_CPUS; cpu ++)
    {
        per_cpu(my_var, cpu) = 0;
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

    printk(KERN_INFO "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
