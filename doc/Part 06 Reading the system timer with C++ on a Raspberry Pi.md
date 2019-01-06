Part 06: Reading the system timer with C++ on a Raspberry Pi
============================================================
The documentation for the system timer can be found in the
[Peripheral specification](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf)
in chapter 12 page 172.

The system timer is a high resolution clock for measuring small time intervals.
It has a resolution of 1 MHz and is therefore able to measure time down to
one microsecond. The system timer consists of a 64 bit counter which is incremented
continuously. The 64 bit are made available through two 32 bit registers providing
the 32 low and 32 high bit.

For measuring small time intervals it is not necessary to read the whole 64 bit
of the counter. This would be difficult because on a 32 bit operating system
(which Raspbian is) there is no guarantee that we could read the high 32 bit
and the low 32 bit of the counter in one go. There would always be the chance
that after having read the low 32 bit the counter would increment the high
32 bit resulting in a completely wrong counter value.

Instead we only ever read the lower 32 bit of the counter and do not worry
about the upper half. As long as we only work with differences between two
counter values which are less than 2^32 / 1000000 / 60 = 71.58 minutes
appart from another we have no problems. Even wrapping around will just
work fine because of unsigned integer arithemtics. This is even the way
the linux kernel works with its own counter (see [Linux Device Drivers, Third Edition](https://lwn.net/Kernel/LDD3/) chapter 7 - Using the jiffies Counter).
And when using time intervals bigger than 71 minutes, there is probably
no need for a high resolution timer and we could simply use the system
time of the operating system.

For source code look for the following files and directories:
* `user-space/raspberrypi`
	* `CSystemTimer.hpp` / `CSystemTimer.cpp`
