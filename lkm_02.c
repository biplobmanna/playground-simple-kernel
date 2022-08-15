#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("biplobmanna");
MODULE_DESCRIPTION("A small Kernel modele with Application API");
MODULE_VERSION("0.01");


// defining the device & data
#define DEVICE_NAME "lkm_02"
#define MESSAGE "Message from lkm_02\n"
#define MESSAGE_BUFFER_LEN 21


// Prototypes for device functions
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);


static int major_num;
static int device_open_count = 0;
static char msg_buffer[MESSAGE_BUFFER_LEN];
static char *msg_ptr;


// structure for containing pointers to device operations
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};


// read from the device
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    int bytes_read = 0;

    // if at the end, loop back to the beginning
    if (*msg_ptr == 0) { msg_ptr = msg_buffer; }

    // put data into the buffer
    while (len && *msg_ptr) {
        // buffer is in user data, not kernel
        // cannot be referenced directly
        // use helper function
        put_user(*(msg_ptr++), buffer++);
        len--;
        bytes_read++;
    }

    return bytes_read;
}


// write to the device
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    // read only device, throw alert & error code
    printk(KERN_ALERT "This operation is not supported!\n");
    return -EINVAL;
}


// open the device
static int device_open(struct inode *inode, struct file *file) {
    if (device_open_count) { return -EBUSY; }

    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}


// release the device
static int device_release(struct inode *inode, struct file *file) {
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}


static int __init lkm_02_init(void) {
    strncpy(msg_buffer, MESSAGE, MESSAGE_BUFFER_LEN);
    msg_ptr = msg_buffer;

    // register character device
    major_num = register_chrdev(0, DEVICE_NAME, &file_ops);

    if (major_num < 0 ) {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    } else {
        printk(KERN_INFO "lkm_02 module loaded with device: %d\n", major_num);
        return 0;
    }
}


static void __exit lkm_02_exit(void) {
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Exiting from lkm_02!\n");
}


// register the module functions
module_init(lkm_02_init);
module_exit(lkm_02_exit);

