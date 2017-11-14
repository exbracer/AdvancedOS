/*************************************************************************
	> File Name: lec04_p51.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月13日 21時47分47秒
 ************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <linux/highmem.h>

#define PRINT_PERF "[HIGHMEM]: "
#define INTS_IN_PAGE (PAGE_SIZE/sizeof(int))

static int __init my_mod_init(void)
{
    struct page *my_page;
    void *my_ptr;
    int i, *int_array;

    printk(PRINT_PERF "Entering module.\n");

    my_page = alloc_page(GFP_HIGHUSER);
    if (!my_page)
    {
        return -1;
    }

    my_ptr = kmap(my_page);
    int_array = (int *) my_ptr;

    for (i = 0; i < INTS_IN_PAGE; i ++)
    {
        int_array[i] = i;
        printk(PRINT_PERF "array[%d] = %d\n", i, int_array[i]);
    }

    kunmap(my_page);
    __free_pages(my_page, 0);

    return 0;
}

static void __exit my_mod_exit(void)
{
    printk(PRINT_PERF "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

