//
// Created by kwh44 on 12/5/19.
//

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>

#include <linux/cdev.h>

#include "gpio_write.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

#define DEVICE_NAME "s7driver"
#define CHARDEV_MINOR        19   /* start of minor numbers requested */
#define CHARDEV_MINOR_NUM    1    /* how many minors requested */


static dev_t first;             /* global variable for the first device number */
static struct cdev c_dev;       /* global variable for the character device structure */
static struct class *cd_class;  /* global variable for the device class */


static  int VALUE = 0;

static int s7driver_open(struct inode *i, struct file *f) {
    printk(KERN_DEBUG "[s7driver] - open() method called\n");
    return 0;
}

static int s7driver_release(struct inode *i, struct file *f) {
    printk(KERN_DEBUG "[s7driver] - close() method called\n");
    return 0;
}

// on read return current number on display
static ssize_t s7driver_read(struct file *f, char __user *buf, size_t len, loff_t*off) {
    char byte = VALUE + '0';
    put_user(byte, buf);
    printk(KERN_DEBUG "[s7driver] - read() method called\n");
    return 1;
}

// on write set number on display


static ssize_t s7driver_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {

    static int mapping[][10] = {{0, 0, 0, 0, 0, 0, 0},
                              {1, 0, 1, 0, 1, 0, 1},

                              {0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0},

                              {0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0},

                              {0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0},

                              {0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0}};
    // read char from user space buffer
    char kbuf;
    if(raw_copy_from_user(&kbuf, buf, 1) != 0) return -EFAULT;
    //
    int value;
    value =  kbuf - '0';
    int i;
    for (i = 0; i < 7; ++i) write_to_pin(i, mapping[value][i]);
    printk(KERN_DEBUG "[s7driver] - write() method called\n");
    return 1;
}

static struct file_operations s7driver_fops =
        {
                .owner = THIS_MODULE,
                .open = s7driver_open,
                .release = s7driver_release,
                .read = s7driver_read,
                .write = s7driver_write
        };



static int __init s7driver_init(void) {
    int ret;
    struct device *dev_ret;

    printk(KERN_DEBUG "[s7driver] - init functions called");
    /* allocate minor numbers */
    if ((ret = alloc_chrdev_region(&first, CHARDEV_MINOR, CHARDEV_MINOR_NUM, DEVICE_NAME)) < 0) {
        return ret;
    }
    /* create class for device */
    if (IS_ERR(cd_class = class_create(THIS_MODULE, DEVICE_NAME))) {
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cd_class);
    }

    /* create device */
    if (IS_ERR(dev_ret = device_create(cd_class, NULL, first, NULL, "foobar"))) {
        class_destroy(cd_class);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(dev_ret);
    }

    /* init cdev sctructure */
    cdev_init(&c_dev, &s7driver_fops);
    /* tell the kernel */
    if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
        device_destroy(cd_class, first);
        class_destroy(cd_class);
        unregister_chrdev_region(first, 1);
        return ret;
    }
    // if cdev_add is successful -- device is live, its operation can be called by the kernel

    return 0;
}

static void __exit s7driver_exit(void) {
    cdev_del(&c_dev);
    device_destroy(cd_class, first);
    class_destroy(cd_class);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "[s7driver] - unregistered from kernel");
}

module_init(s7driver_init);
module_exit(s7driver_exit);
