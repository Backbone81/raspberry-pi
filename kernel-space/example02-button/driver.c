#include <linux/module.h> // needed for MODULE_LICENSE, module_init, module_exit
#include <linux/cdev.h> // needed for cdev_init, cdev_add, cdev_del, struct cdev
#include <linux/slab.h> // needed for kmalloc, kfree
#include <linux/uaccess.h> // needed for copy_to_user
#include <linux/fs.h> // needed for file_operations
#include <linux/delay.h> // needed for mdelay
#include <linux/interrupt.h> // needed for request_irq, free_irq
#include <linux/gpio/consumer.h> // needed for GPIO

MODULE_LICENSE("Dual MIT/GPL");

#include "logger.h"

static dev_t device_number;
static struct cdev my_cdev;

#define INPUT_PIN_NUM 27
#define OUTPUT_PIN_NUM 17

static struct gpio_desc* input_pin = NULL;
static struct gpio_desc* output_pin = NULL;
static int input_pin_irq = 0;
static void* unique_irq_flag = NULL;

static int my_open(struct inode* inode, struct file* filep);
static ssize_t my_read(struct file* filep, char __user* buff, size_t count, loff_t* offp);
static int my_release(struct inode *inode, struct file* filep);

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .release = my_release,
};

static irqreturn_t my_interrupt_handler(int irq, void *dev_id)
{
    int curr_value = 0;
    DEBUG_OUT("Interrupt handler called (IRQ %02X, DEV %p)", irq, dev_id);
    if (dev_id != unique_irq_flag)
    {
        // not our interrupt handler
        DEBUG_OUT("Not our handler");
        return IRQ_NONE;
    }

    curr_value = gpiod_get_value(input_pin);
    if (curr_value < 0)
    {
        ERROR_OUT("Unable to retrieve current value of input GPIO pin");
        return IRQ_HANDLED;
    }
    gpiod_set_value(output_pin, curr_value);
    return IRQ_HANDLED;
}

static int my_open(struct inode* inode, struct file* filep)
{
    DEBUG_OUT("Calling open");
    // no implementation
    return 0;
}

static ssize_t my_read(struct file* filep, char __user* buff, size_t count, loff_t* offp)
{
    DEBUG_OUT("Calling read");
    // no implementation
    return 0;
}

static int my_release(struct inode *inode, struct file* filep)
{
    DEBUG_OUT("Calling release");
    // no implementation
    return 0;
}

static int my_init(void)
{
    int error;
    DEBUG_OUT("Driver init");

    // allocate device number
    error = alloc_chrdev_region(&device_number, 0, 1, "example02-button");
    if (error)
    {
        ERROR_OUT("Unable to allocate character device number");
        return -1;
    }

    // get a unique irq flag
    unique_irq_flag = kmalloc(1, GFP_KERNEL);
    if (unique_irq_flag == NULL)
    {
        ERROR_OUT("Unable to allocate our unique IRQ flag");
        unregister_chrdev_region(device_number, 1);
        return -1;
    }

    // prepare GPIO input
    input_pin = gpio_to_desc(INPUT_PIN_NUM);
    if (input_pin == NULL)
    {
        ERROR_OUT("Unable to get input pin GPIO descriptor");
        goto fail;
    }
    if (gpiod_direction_input(input_pin))
    {
        ERROR_OUT("Unable to set input pin GPIO direction");
        goto fail;
    }

    // prepare GPIO output
    output_pin = gpio_to_desc(OUTPUT_PIN_NUM);
    if (output_pin == NULL)
    {
        ERROR_OUT("Unable to get output pin GPIO descriptor");
        goto fail;
    }
    if (gpiod_direction_output(output_pin, false))
    {
        ERROR_OUT("Unable to set output pin GPIO direction");
        goto fail;
    }

    // install interrupt
    input_pin_irq = gpiod_to_irq(input_pin);
    if (input_pin_irq < 0)
    {
        ERROR_OUT("Unable to retrieve IRQ number for input pin");
        goto fail;
    }
    error = request_irq(input_pin_irq, my_interrupt_handler, IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "example02-button", unique_irq_flag);
    if (error)
    {
        ERROR_OUT("Unable to request interrupt");
        goto fail;
    }

    // register device operations
    cdev_init(&my_cdev, &my_fops);
    my_cdev.owner = THIS_MODULE;
    error = cdev_add(&my_cdev, device_number, 1);
    if (error)
    {
        ERROR_OUT("Unable to add character device");
        free_irq(input_pin_irq, unique_irq_flag);
        goto fail;
    }
    return 0;

fail:
    if (output_pin != NULL)
    {
        gpiod_put(output_pin);
    }
    if (input_pin != NULL)
    {
        gpiod_put(input_pin);
    }
    if (unique_irq_flag != NULL)
    {
        kfree(unique_irq_flag);
    }
    unregister_chrdev_region(device_number, 1);
    return -1;
}

static void my_exit(void)
{
    DEBUG_OUT("Driver exit");
    cdev_del(&my_cdev);
    free_irq(input_pin_irq, unique_irq_flag);
    gpiod_put(output_pin);
    gpiod_put(input_pin);
    kfree(unique_irq_flag);
    unregister_chrdev_region(device_number, 1);
}

module_init(my_init);
module_exit(my_exit);
