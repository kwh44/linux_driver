//
// Created by kwh44 on 12/10/19.
//



// includes linux kernel headers


#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#include "gpio_write.h"

const static int pins[] = {4, 17, 18, 27, 22, 23, 24};

void write_to_pin(int pin, int value) {
    gpio_direction_output(pins[pin], value);
}

static int __init

md_init(void) {
    int i = 0;
    for (; i < 7; ++i) gpio_direction_output(pins[i], 0);
    printk("gpio_write module loaded!\n");
    return 0;
}

static void __exit
md_exit(void) {
    printk("gpio_write module disloaded\n");
}

EXPORT_SYMBOL(write_to_pin);

MODULE_LICENSE("GPL");

module_init(md_init);
module_exit(md_exit);
