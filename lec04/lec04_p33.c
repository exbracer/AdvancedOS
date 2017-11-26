/*************************************************************************
	> File Name: lec04_p33.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月13日 16時14分59秒
 ************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

#define PRINT_PERF "[KMALLOC_TEST]: "

static int __init my_mod_init(void)
{
    unsigned long i;
    void *ptr;

    printk(PRINT_PERF "Entering module.\n");

    for(i=1;;i*=2)
    {
        ptr = kmalloc(i, GFP_KERNEL);
        if(!ptr)
        {
            printk(PRINT_PERF "could not allocate %lu bytes\n", i);
            break;
        }
        kfree(ptr);
    }

    return 0;
}

static void __exit my_mod_exit(void)
{
    printk(KERN_INFO "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
