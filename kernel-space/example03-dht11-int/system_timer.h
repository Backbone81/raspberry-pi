#pragma once

#include <linux/types.h> // needed for u32

/*
 * Prepare the system timer for usage
 */
int system_timer_init(void);

/*
 * Cleanup the system timer, we are done
 */
void system_timer_shutdown(void);

/*
 * Returns the current tick count of the system timer
 */
u32 system_timer_ticks(void);

/*
 * Returns the frequency of the system timer as ticks per second
 */
u32 system_timer_frequency(void);
