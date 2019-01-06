Part 13: Reading a DHT11 sensor with C as a linux device driver on a Raspberry Pi - temperature and humidity
============================================================================================================
In "Part 08: Reading a DHT11 sensor with C++ on a Raspberry Pi - temperature and humidity" we learned how
to read a DHT11 sensor and noticed some timing problems when this reading happened in user space. This
is because the operating system is free to suspend any process at any time and run some other process.
This can cause missing data while reading the DHT11 sensor. To fix this issue, we will create a linux
device driver which provides a character device which user space applications can read for accessing
data.

Developing linux device drivers is a complex topic. The free book [Linux Device Drivers, Third Edition](https://lwn.net/Kernel/LDD3/)
helped a lot in understanding the architecture and principles in linux kernel space. Note that this book
is written for linux kernel version 2.6.10 whereas we currently have 4.20. Most of what is described in
this book is still up to date, but some functions might be in different header files now or are deprecated
in favor of other functions. I highly recommend downloading the current linux kernel sources and having
a look into its "Documentation" subdirectory, its header files and function implementation. The documentation
found in the "Documentation" subdirectory usually provides a high level overview of some topics, whereas
detailed function documentation is usually found at the location where the function is implemented. If you
want to look up some specific function, simply search all .h or all .c files for its name and you will
find useful information.

From all the chapters of Linux Device Drivers, Third Edition the following chapters were important to me:

* Chapter 2: Building and Running Modules
* Chapter 3: Char Drivers
* Chapter 5: Concurrency and Race Conditions
* Chapter 7: Time, Delays, and Deferred Work
* Chapter 8: Allocating Memory
* Chapter 9: Communicating with Hardware
* Chapter 10: Interrupt Handling
* Chapter 11: Data Types in the Kernel

As expected, kernel space code is not suspended which provides perfect reliability when communicating
with the DHT11 sensor. The example code at "kernel-space\example01-dht11" provides a file at /dev/example01-dht11
which can be read and returns the current temperature and humidity:

```
pi@rasppi1:~ $ cat /dev/example01-dht11
Temperature: 23 / Humidity: 58
pi@rasppi1:~ $
```

Note that you need to have the package "raspberrypi-kernel-headers" installed to be able to compile
the kernel module.

The DHT11 data is only queried when opening the file. You need to reopen the file to get updated
sensor data. Concurrent openings will cause delays to satisfy the 2 second minimum time between
measurements.

**NOTE: This implementation is a near 1:1 port of the C++ code from part 08. This means we are directly
accessing the memory of our GPIO pins. This memory is already in use by a linux kernel driver for GPIO
and we will get a warning when we install our own driver. It would be better to use the linux kernel
GPIO library to access our GPIO pins. But for the sake of simplicity and comparability we will stick
to the 1:1 port in this part of the documentation.**

**NOTE: This implementation is still using busy waiting while communicating with the DHT11 sensor.
This is bad because it wastes cpu cycles while waiting for the communication to continue. This
could probably be improved with hardware interrupts which signal when a GPIO pin changes from
low to high or high to low.**

For source code look for the following files and directories:
* `kernel-space\example01-dht11`
	* `README.md`
