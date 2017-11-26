/*************************************************************************
	> File Name: keyboard.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月25日 21時28分10秒
 ************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/interrupt.h>

#include <linux/sched.h>

#include <asm/io.h>

#define PRINT_PERF "[KEYBOARD]: "

/*
static void print_key(void *scancode)
{
    printk(PRINT_PERF "Scan Code %x %s.\n", (int)*((char*) scancode) & 0x7F, *((char *)scancode) & 0x80 ? "Released" : "Pressed");

}
*/

static irqreturn_t irq_handler(int irq, void * dev_id)
{
    static unsigned char scancode;
    unsigned char status;

    status = inb(0x64);
    scancode = inb(0x60);

    printk(PRINT_PERF "Scan Code %x %s.\n", (int)scancode & 0x7F, scancode & 0x80 ? "Released" : "Pressed");

    return IRQ_HANDLED;

}
static int __init kb_irq_mod_init(void)
{
    free_irq(1, NULL);
    int ret;
    printk(PRINT_PERF "Entering module.\n");
   
    ret = request_irq(1, (irq_handler_t) irq_handler, IRQF_SHARED, "keyboard_interrupt_handler", (void*)irq_handler);
    return 0;
}

static void __exit kb_irq_mod_exit(void)
{
    free_irq(1, (void*)irq_handler);
    printk(PRINT_PERF "Exiting module.\n");
}

module_init(kb_irq_mod_init);
module_exit(kb_irq_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yuchen QIAO");
