Example 03 DHT11 Interrupt
==========================
This project provides a device driver for reading temperature and humidity data
from a DHT11 sensor.

For the Makefile to build this project, you need the linux kernel header files
installed. On Raspbian this requires the apt package "raspberrypi-kernel-headers".

The shell script files "load.sh" and "unload.sh" can be used to install and
uninstall the driver temporarily.

The "driver.c" file is the main entry point for the linux device driver.

The Visual Studio project uses a Linux Makefile project for developing on Windows
but building on Linux.

This device driver provides a device file at /dev/example03-dht11-int. This
file contains the latest measurement data. A simple example looks like this:

```
pi@rasppi1:~ $ cat /dev/example03-dht11-int
Temperature: 23 / Humidity: 58
pi@rasppi1:~ $
```

Note that the data is fixed when the file is being opened. The content of
this file does not change any more for the reader. The file needs to be closed
and reopened again for new data to be returned.

Multiple processes can open and reas the file concurrently. All processes
will immediately have access to the latest measurement data and are not blocked.

While one process has the file open for reading, other processes will get
different data when they open that file. The measurement data is specific
to each open.

The data communication with the DHT11 sensor is done through hardware interrupts
which enable minimum load on the processor and maximum performance.

**NOTE: The start sequence for measuring the DHT11 sensor data still has a
19 millisecond busy wait which is not optimal. But it is done asynchroneous
and does not block anyone reading the device file.**
