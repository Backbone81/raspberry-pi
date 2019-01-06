#pragma once

#include <linux/kernel.h> // needed for printk

// comment out the next line to disable debug output
// #define ENABLE_DEBUG_OUT

// DEBUG_OUT prints debug information into the kernel log only when ENABLE_DEBUG_OUT is defined
// otherwise those debug outputs will be optimized away
#ifdef ENABLE_DEBUG_OUT
#define DEBUG_OUT(message, ...) printk(KERN_DEBUG message "\n", ##__VA_ARGS__)
#else
#define DEBUG_OUT(message, ...)
#endif

// write warnings into the kernel log
#define WARNING_OUT(message, ...) printk(KERN_WARNING message "\n", ##__VA_ARGS__)

// write error messages into the kernel log
#define ERROR_OUT(message, ...) printk(KERN_ALERT message "\n", ##__VA_ARGS__)
