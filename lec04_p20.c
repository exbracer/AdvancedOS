/*************************************************************************
    > File Name: lec04p20.c
    > Author: qiao_yuchen
    > Mail: qiaoyc14@mails.tsinghua.edu.cn 
    > Created Time: Fri Nov 10 16:17:22 2017
 ************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gfp.h>

#define PRINT_PERF "[LOWLEVEL]: "
#define PAGES_ORDER_REQUESTED 3
#define INTS_IN_PAGE (PAGE_SIZE/sizeof(int))

unsigned long virt_addr;

static int __init my_mod_init(void)
{
	int *int_array;
	int i;

	printk(PRINT_PERF "Entering module.");

    virt_addr = __get_free_pages(GFP_KERNEL, PAGES_ORDER_REQUESTED);
    if (!virt_addr)
    {
        printk(PRINT_PERF "Error in allocation \n ");
        return -1;
    }

    int_array = (int *)virt_addr;
    for (i = 0; i < INTS_IN_PAGE; i ++)
    {
        int_array[i] = i;
    }

    for (i = 0; i < INTS_IN_PAGE; i ++)
    {
        printk(PRINT_PERF "array[%d] = %d\n", i, int_array[i]);
    }

    return 0;
}

static void __exit my_mod_exit(void)
{
    free_pages(virt_addr, PAGES_ORDER_REQUESTED);
    printk(PRINT_PERF "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);
