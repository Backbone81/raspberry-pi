#pragma once

#include <linux/types.h> // needed for bool

/*
 * Prepare GPIO for usage
 */
int gpio_init(void);

/*
 * Cleanup GPIO, we are done
 */
void gpio_shutdown(void);

/*
 * Set the pin provided as parameter as input pin
 */
void gpio_set_input(int pin);

/*
 * Set the pin provided as parameter as output pin
 */
void gpio_set_output(int pin);

/*
 * Set the pin provided as parameter to high
 */
void gpio_set_high(int pin);

/*
 * Set the pin provided as parameter to low
 */
void gpio_set_low(int pin);

/*
 * Set the pin provided as parameter to high or low depending on the second parameter
 */
void gpio_set(int pin, bool high);

/*
 * Returns true if the pin provided as parameter is currently high
 */
bool gpio_is_high(int pin);

/*
 * Returns true if the pin provided as parameter is currently low
 */
bool gpio_is_low(int pin);
