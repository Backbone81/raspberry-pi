#include <linux/module.h> // needed for MODULE_LICENSE, module_init, module_exit
#include <linux/cdev.h> // needed for cdev_init, cdev_add, cdev_del, struct cdev
#include <linux/slab.h> // needed for kmalloc, kfree
#include <linux/uaccess.h> // needed for copy_to_user
#include <linux/fs.h> // needed for file_operations

MODULE_LICENSE("Dual MIT/GPL");

#include "logger.h"
#include "system_timer.h"
#include "dht11.h"

// the GPIO pin we use for communication with the DHT11 sensor
#define DATA_PIN_NUM 17

// the size of the buffer we allocate for each file open operation and which gets the last measurement copied into
#define BUFFER_SIZE 64

static dev_t device_number;
static struct cdev my_cdev;

static int my_open(struct inode* inode, struct file* filep);
static ssize_t my_read(struct file* filep, char __user* buff, size_t count, loff_t* offp);
static int my_release(struct inode *inode, struct file* filep);

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .release = my_release,
};

static int my_open(struct inode* inode, struct file* filep)
{
    struct dht11_data my_data;
    DEBUG_OUT("Calling open");

    // allocate individual data buffer
    filep->private_data = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (filep->private_data == NULL)
    {
        ERROR_OUT("Unable to allocate private data");
        return -1;
    }
    memset(filep->private_data, 0, BUFFER_SIZE);

    // write data into buffer
    my_data = dht11_measure();
    snprintf(filep->private_data, BUFFER_SIZE, "Temperature %i C / Humidity %i %%\n", my_data.temperature, my_data.humidity);

    return 0;
}

static ssize_t my_read(struct file* filep, char __user* buff, size_t count, loff_t* offp)
{
    size_t data_size = strlen(filep->private_data);
    DEBUG_OUT("Calling read");

    if (data_size < *offp)
    {
        // the buffer was already read to its end
        return 0;
    }

    if (data_size < *offp + count)
    {
        // it was more data requested than available in the buffer
        // reduce the count to a valid value
        count = data_size - *offp;
    }

    // copy data over to the caller
    if (copy_to_user(buff, filep->private_data + *offp, count) != 0)
    {
        ERROR_OUT("Unable to copy data from kernel to user space.");
        return 0;
    }
    *offp += count;
    return count;
}

static int my_release(struct inode *inode, struct file* filep)
{
    DEBUG_OUT("Calling release");
    kfree(filep->private_data);
    return 0;
}

static int my_init(void)
{
    int error;
    DEBUG_OUT("Driver init");

    // allocate device number
    error = alloc_chrdev_region(&device_number, 0, 1, "example03-dht11-int");
    if (error)
    {
        ERROR_OUT("Unable to allocate character device number");
        return -1;
    }

    // init system timer
    error = system_timer_init();
    if (error)
    {
        ERROR_OUT("Unable to initialize system timer");
        unregister_chrdev_region(device_number, 1);
        return -1;
    }

    // init dht11
    error = dht11_init(DATA_PIN_NUM);
    if (error)
    {
        ERROR_OUT("Unable to initialize DHT11");
        system_timer_shutdown();
        unregister_chrdev_region(device_number, 1);
        return -1;
    }

    // register device operations
    cdev_init(&my_cdev, &my_fops);
    my_cdev.owner = THIS_MODULE;
    error = cdev_add(&my_cdev, device_number, 1);
    if (error)
    {
        ERROR_OUT("Unable to add character device");
        dht11_shutdown();
        system_timer_shutdown();
        unregister_chrdev_region(device_number, 1);
        return -1;
    }
    return 0;
}

static void my_exit(void)
{
    DEBUG_OUT("Driver exit");
    cdev_del(&my_cdev);
    dht11_shutdown();
    system_timer_shutdown();
    unregister_chrdev_region(device_number, 1);
}

module_init(my_init);
module_exit(my_exit);
