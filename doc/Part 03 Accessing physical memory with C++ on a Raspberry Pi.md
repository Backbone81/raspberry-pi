Part 03: Accessing physical memory with C++ on a Raspberry Pi
=============================================================
For getting a first idea about how to access the Raspberry Pi hardware from C++
[Low Level Programming of the Raspberry Pi in C](http://pieter-jan.com) was a very
helpful resource.

The different components of the Rapsberry Pi are accessed through fixed physical
memory addresses which are documented in the
[Peripheral specification](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf).
Note that depending on which model of the Rapsberry Pi you use, the base address of
the peripheries is different. See [Peripheral Addresses](https://www.raspberrypi.org/documentation/hardware/raspberrypi/peripheral_addresses.md)
for details.

For accessing physical memory locations from user space on linux operating systems
you have to access /dev/mem. Access to this file requires root privileges which means
you have to run your C++ application with sudo.

Because /dev/mem acts as a file, you would have to use file seek, read and write
operations to change the content of a physical memory location. To make things
easier, just memory map portions of that file into the address space of your
own programm. Afterwards you can simply read and write memory locations through
pointers and the operating system will redirect those actions to the physical
memory location.

For source code look in the directory `user-space/raspberrypi` for the following files:
* `CPhysicalMemory.hpp` / `CPhysicalMemory.cpp`
* `CMemoryMapper.hpp` / `CMemoryMapper.cpp`
* `CRaspberryPi.hpp` / `CRaspberryPi.cpp`
