/*************************************************************************
	> File Name: hello-1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月12日 21時20分21秒
 ************************************************************************/

#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed by KERN_INFO */

int init_module(void)
{
    printk(KERN_INFO "Hello, world 1.\n");

    /*
     * A non 0 return means init_module fails; module can't be loaded.
     */

    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "Goodbye world 1.\n");
}
