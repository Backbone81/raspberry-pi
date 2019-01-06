#pragma once

#include <cstdint>

/* 
 * This class provides access to the Raspberry Pi system timer. This is a clock with 1 MHz resolution.
 * Use this system timer when you need to measure microsecond times.
 *
 * The hardware documentation can be found on the BCM2835 ARM Peripherals specification at
 * https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf
 * in chapter 12 on page 172.
 */
class CSystemTimer
{
public:
    /*
     * No need for copying objects of this class
     */
    CSystemTimer(const CSystemTimer&) = delete;
    CSystemTimer& operator=(const CSystemTimer&) = delete;

public:
    /*
     * Constructor
     * Provide the physical memory address to the system timer as address.
     */
    explicit CSystemTimer(void* Base);

    /*
     * Returns the current number of ticks of the system timer.
     *
     * NOTE: This only returns the lower 32 bit of the 64 bit system timer. This is no problem as long
     *       as you only use this value for calculating differences in time elapsed which are less than
     *       71 minutes (2^32 / 1000000 / 60). Returning the whole 64 bit system timer would have an
     *       impact on performance and would require extra care for getting a consistent value on overflowing
     *       situations.
     */
    uint32_t GetTicks() const;

    /*
     * Returns the number of ticks per second the system timer provides.
     */
    uint32_t GetFrequency() const;

private:
    volatile uint32_t* m_Register; /* the base memory address */

    volatile uint32_t* m_Counter; /* the the lower 32 bit of the counter*/

}; // class CSystemTimer
