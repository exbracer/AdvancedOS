/*************************************************************************
	> File Name: hello-1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月12日 21時20分21秒
 ************************************************************************/
/*
 * hello-2.c - Demonstrating module_init() and model_exit() macros.
 * This is preferred over using init_module() and cleanup_module().
 */

#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros. */

static int __init hello_2_init(void)
{
    printk(KERN_INFO "Hello, world 2.\n");

    return 0;
}

static void __exit hello_2_exit(void)
{
    printk(KERN_INFO "Goodbye world 2.\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);
