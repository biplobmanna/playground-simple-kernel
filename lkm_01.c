#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("biplobmanna");
MODULE_DESCRIPTION("Getting started with a simple Linux Kernel Module");
MODULE_VERSION("0.01");

// init function, where it all starts
static int __init lkm_01_init(void) {
    printk(KERN_INFO "Hello, World!");
    return 0;
}

// exit function, where it all ends
static void  __exit lkm_01_exit(void) {
    printk(KERN_INFO "Adios, amigos!");
}

module_init(lkm_01_init);
module_exit(lkm_01_exit);
