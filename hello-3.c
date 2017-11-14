/*************************************************************************
	> File Name: hello-3.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月12日 21時20分21秒
 ************************************************************************/
/*
 * hello-3.c - Illustrating the __init, __initdata and __exit macros.
 */

#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros. */

static int hello_3_data __initdata = 3;

static int __init hello_3_init(void)
{
    printk(KERN_INFO "Hello, world %d.\n", hello_3_data);

    return 0;
}

static void __exit hello_3_exit(void)
{
    printk(KERN_INFO "Goodbye world %d.\n", hello_3_data);
}

module_init(hello_3_init);
module_exit(hello_3_exit);