#pragma once

#include "CPhysicalMemory.hpp"
#include "CMemoryMapper.hpp"
#include "CGpio.hpp"
#include "CSystemTimer.hpp"
#include "CBsc.hpp"

/*
 * This class provides an abstraction for the Raspberry Pi. It returns objects for accessing its peripheral hardware.
 */
class CRaspberryPi
{
public:
    /*
     * The hardware documentation can be found on the BCM2835 ARM Peripherals specification at
     * https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf
     * in chapter 1.2.3 on page 6. It states that the physical address is 0x20000000 but at
     * https://www.raspberrypi.org/documentation/hardware/raspberrypi/peripheral_addresses.md the documentation
     * explains that this base address is dependent on the Raspberry Pi model used. I use a Raspberry Pi 3
     * therefore this base address is 0x3F000000 for me.
     */
    const std::size_t BCM2835_PERIPHERALS_BASE = 0x3F000000;

    const std::size_t GPIO_OFFSET = 0x00200000; /* datasheet: 6.1 Register View (page 90) */
    const std::size_t GPIO_BASE = BCM2835_PERIPHERALS_BASE + GPIO_OFFSET;
    const std::size_t GPIO_SIZE = 41 * sizeof(uint32_t);

    const std::size_t SYSTEM_TIMER_OFFSET = 0x00003000; /* datasheet: 12 System Timer (page 172) */
    const std::size_t SYSTEM_TIMER_BASE = BCM2835_PERIPHERALS_BASE + SYSTEM_TIMER_OFFSET;
    const std::size_t SYSTEM_TIMER_SIZE = 7 * sizeof(uint32_t);

    const std::size_t BSC0_OFFSET = 0x205000; /* datasheet: 3.2 Register View (page 28) */
    const std::size_t BSC0_BASE = BCM2835_PERIPHERALS_BASE + BSC0_OFFSET;
    const std::size_t BSC1_OFFSET = 0x804000; /* datasheet: 3.2 Register View (page 28) */
    const std::size_t BSC1_BASE = BCM2835_PERIPHERALS_BASE + BSC1_OFFSET;
    const std::size_t BSC_SIZE = 8 * sizeof(uint32_t);

public:
    /*
     * No need for copying objects of this class
     */
    CRaspberryPi(const CRaspberryPi&) = delete;
    CRaspberryPi& operator=(const CRaspberryPi&) = delete;

public:
    /*
     * Constructor
     * Initializes all components
     */
    CRaspberryPi();

    /*
     * Destructor
     */
    ~CRaspberryPi();

    /*
     * Returns the system timer object
     */
    CSystemTimer& SystemTimer();

    /*
     * Returns the GPIO object
     */
    CGpio& Gpio();

    /*
     * Returns the BSC0 object for I2C
     */
    CBsc& Bsc0();

    /*
     * Returns the BSC1 object for I2C
     */
    CBsc& Bsc1();

private:
    CPhysicalMemory m_PhysicalMemory;

    CMemoryMapper m_SystemTimerMemory;
    CSystemTimer m_SystemTimer;

    CMemoryMapper m_GpioMemory;
    CGpio m_Gpio;

    CMemoryMapper m_Bsc0Memory;
    CBsc m_Bsc0;

    CMemoryMapper m_Bsc1Memory;
    CBsc m_Bsc1;

};
