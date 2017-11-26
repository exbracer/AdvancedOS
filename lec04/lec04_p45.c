/*************************************************************************
	> File Name: lec04_p45.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月13日 21時01分45秒
 ************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

#define PRINT_PERF "[SLAB_TEST]: "

struct my_struct
{
    int int_param;
    long long_param;
};

static int __init my_mod_init(void)
{
    int ret = 0;
    struct my_struct * ptr1, * ptr2;
    struct kmem_cache * my_cache;

    printk(PRINT_PERF "Entering module.\n");

    my_cache = kmem_cache_create("pierre-cache", sizeof(struct my_struct), 0, 0, NULL);

    if (!my_cache)
    {
        return -1;
    }

    ptr1 = kmem_cache_alloc(my_cache, GFP_KERNEL);

    if (!ptr1)
    {
        ret = -ENOMEM;
        goto destroy_cache;
    }

    ptr2 = kmem_cache_alloc(my_cache, GFP_KERNEL);
    if (!ptr2)
    {
        ret = -ENOMEM;
        goto freeptr1;
    }

    ptr1->int_param = 42;
    ptr1->long_param = 42;
    ptr2->int_param = 43;
    ptr2->long_param = 43;

    printk(PRINT_PERF "ptr1 = {%d, %ld}; ptr2 = {%d, %ld}\n", ptr1->int_param, ptr1->long_param, ptr2->int_param, ptr2->long_param);
    kmem_cache_free(my_cache, ptr2);

freeptr1:
    kmem_cache_free(my_cache, ptr1);

destroy_cache:
    kmem_cache_destroy(my_cache);

    return ret;
}

static void __exit my_mod_exit(void)
{
    printk(PRINT_PERF, "Exiting module.\n");
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
