#include "gpio.h"

#include <linux/ioport.h> // needed for request_mem_region, release_mem_region
#include <linux/io.h> // needed for ioremap, iounmap

#include "common.h"
#include "logger.h"

#define GPIO_OFFSET 0x00200000
#define GPIO_BASE (BCM2835_PERIPHERALS_BASE + GPIO_OFFSET)
#define GPIO_SIZE (41 * sizeof(u32))

static u32* gpio_base = NULL;
static u32* gpio_function_select = NULL;
static u32* gpio_pin_output_set = NULL;
static u32* gpio_pin_output_clear = NULL;
static u32* gpio_pin_level = NULL;

int gpio_init(void)
{
    DEBUG_OUT("GPIO init");
    if (request_mem_region(GPIO_BASE, GPIO_SIZE, "example01-dht11") == NULL)
    {
        // This will always fail. The linux kernel has a GPIO driver which already uses
        // this memory reagion. Multiple device drivers manipulating the same memory
        // region is always a bad idea and can lead to suptle or big problems.
        // It would be better to use the GPIO library of the linux kernel.
        WARNING_OUT("The memory region for GPIO is already in use by some other kernel driver. This could lead to unexpected issues.");
    }

    gpio_base = (u32*)ioremap(GPIO_BASE, GPIO_SIZE);
    if (gpio_base == NULL)
    {
        ERROR_OUT("Unable to remap GPIO memory.");
        release_mem_region(GPIO_BASE, GPIO_SIZE);
        return -1;
    }

    gpio_function_select = gpio_base;
    gpio_pin_output_set = gpio_base + 7;
    gpio_pin_output_clear = gpio_base + 10;
    gpio_pin_level = gpio_base + 13;
    return 0;
}

void gpio_shutdown(void)
{
    DEBUG_OUT("GPIO shutdown");
    iounmap(gpio_base);
    release_mem_region(GPIO_BASE, GPIO_SIZE);
}

void gpio_set_input(int pin)
{
    int register_index = pin / 10;
    int bit_offset = (pin % 10) * 3;
    u32 old_value = ioread32(gpio_function_select + register_index);
    iowrite32(old_value & ~(0x7 << bit_offset), gpio_function_select + register_index);
}

void gpio_set_output(int pin)
{
    int register_index = pin / 10;
    int bit_offset = (pin % 10) * 3;
    u32 old_value = ioread32(gpio_function_select + register_index);
    iowrite32((old_value & ~(0x7 << bit_offset)) | (0x1 << bit_offset), gpio_function_select + register_index);
}

void gpio_set_high(int pin)
{
    int register_index = pin / 32;
    int bit_offset = pin % 32;
    iowrite32((0x1 << bit_offset), gpio_pin_output_set + register_index);
}

void gpio_set_low(int pin)
{
    int register_index = pin / 32;
    int bit_offset = pin % 32;
    iowrite32((0x1 << bit_offset), gpio_pin_output_clear + register_index);
}

void gpio_set(int pin, bool high)
{
    if (high)
    {
        gpio_set_high(pin);
    }
    else
    {
        gpio_set_low(pin);
    }
}

bool gpio_is_high(int pin)
{
    int register_index = pin / 32;
    int bit_offset = pin % 32;
    return ioread32(gpio_pin_level + register_index) & (0x1 << bit_offset);
}

bool gpio_is_low(int pin)
{
    return !gpio_is_high(pin);
}
