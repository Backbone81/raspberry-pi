Example 01 DHT11
================
This project provides a device driver for reading temperature and humidity data
from a DHT11 sensor.

For the Makefile to build this project, you need the linux kernel header files
installed. On Raspbian this requires the apt package "raspberrypi-kernel-headers".

The shell script files "load.sh" and "unload.sh" can be used to install and
uninstall the driver temporarily.

The "driver.c" file is the main entry point for the linux device driver.

The Visual Studio project uses a Linux Makefile project for developing on Windows
but building on Linux.

This device driver provides a device file at /dev/example01-dht11. On opening
this file, the DHT11 sensor is queried for measurement data. On subsequent reads
this data is returned as text. A simple example looks like this:

```
pi@rasppi1:~ $ cat /dev/example01-dht11
Temperature: 23 / Humidity: 58
pi@rasppi1:~ $
```

Note that the sensor is queried only on opening the file. While being open, the
content of this file does not change any more. The file needs to be closed
and reopened again for new data to be returned.

Multiple processes can open and read the file concurrently. All open events
will be serialized and respect the 2 second minimum time between measurements.
This means that open events will possibly block for a long time when enough
concurrent processes are opening the file.

While one process has the file open for reading, other processes will get
different data when they open that file. The measurement data is specific to
each open.
