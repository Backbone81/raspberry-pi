#include "system_timer.h"

#include <linux/ioport.h> // needed for request_mem_region, release_mem_region
#include <linux/io.h> // needed for ioremap, iounmap

#include "common.h"
#include "logger.h"

#define SYSTEM_TIMER_OFFSET 0x00003000
#define SYSTEM_TIMER_BASE (BCM2835_PERIPHERALS_BASE + SYSTEM_TIMER_OFFSET)
#define SYSTEM_TIMER_SIZE (7 * sizeof(u32))

static u32* system_timer_base = NULL;
static u32* system_timer_counter = NULL;

int system_timer_init(void)
{
    DEBUG_OUT("System timer init");
    if (request_mem_region(SYSTEM_TIMER_BASE, SYSTEM_TIMER_SIZE, "system timer") == NULL)
    {
        WARNING_OUT("The memory region for the system timer is already in use by some other kernel driver. This could lead to unexpected issues.");
    }

    system_timer_base = (u32*)ioremap(SYSTEM_TIMER_BASE, SYSTEM_TIMER_SIZE);
    if (system_timer_base == NULL)
    {
        ERROR_OUT("Unable to remap system timer memory.");
        release_mem_region(SYSTEM_TIMER_BASE, SYSTEM_TIMER_SIZE);
        return -1;
    }

    system_timer_counter = system_timer_base + 1;
    return 0;
}

void system_timer_shutdown(void)
{
    DEBUG_OUT("System timer shutdown");
    iounmap(system_timer_base);
    release_mem_region(SYSTEM_TIMER_BASE, SYSTEM_TIMER_SIZE);
}

u32 system_timer_ticks(void)
{
    return ioread32(system_timer_counter);
}

u32 system_timer_frequency(void)
{
    return 1000000;
}
