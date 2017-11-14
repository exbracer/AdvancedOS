/*************************************************************************
	> File Name: hello-4.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月12日 21時20分21秒
 ************************************************************************/
/*
 * hello-4.c - Demonstrates module documentation.
 */

#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros. */
#define DRIVER_AUTHOR "Yuchen QIAO <qiao@eidos.ic.i.u-tokyo.ac.jp>"
#define DRIVER_DESC "A sample driver"

static int __init hello_4_init(void)
{
    printk(KERN_INFO "Hello, world 4.\n");

    return 0;
}

static void __exit hello_4_exit(void)
{
    printk(KERN_INFO "Goodbye world 4.\n");
}

module_init(hello_4_init);
module_exit(hello_4_exit);

/*
 * You can use strings, like this:
 */

/*
 * Get rid of taint message by declaring code as GPL.
 */

MODULE_LICENSE("GPL");

/*
 * Or with defines, like this:
 */

MODULE_AUTHOR(DRIVER_AUTHOR); /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC); /* What does this module do */

/*
 * This module uses /dev/testdevice. The MODULE_SUPPORTED_DEVICE macro might
 * be used in the future to help automatic configuration of modules, but is 
 * currently unused other than for documentation purposes.
 */

MODULE_SUPPORTED_DEVICE("testdevice");
