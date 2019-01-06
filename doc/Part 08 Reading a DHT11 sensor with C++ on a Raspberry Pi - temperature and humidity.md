Part 08: Reading a DHT11 sensor with C++ on a Raspberry Pi - temperature and humidity
=====================================================================================
I have bought the DHT11 sensor from [AZ-Delivery](https://www.az-delivery.de) where they also provide the
[datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/DHT_11_Temperatursensor_Modul_Datenblatt_23da69ca-d8c8-41d6-b0c0-b6c6c9897a9e.pdf?12682177290679431465).

DHT11 is a sensor which provides temperature and humidity data. There is a single data line
which is used for communication. When we want to read the data, we have to send a start signal
and then wait for 40 bit of data to be returned by the sensor.

The setup looks like this:

![image](images/part%2008%20wiring.png)

The data pin uses a pull up resistor with 5.1K Ohm according to the datasheet.

Data transfer is done by signaling 0 and 1 data bits. A 0 data bit is signaled by the
sensor pulling the data line down to ground and holding it for 50 microseconds.
Afterwards the data line is pulled up again for 26 microseconds. A 1 data bit is signaled
by pulling down for 50 microseconds and pulling up by 70 microseconds. We can see
the first pull down is always identical and the information about being a 0 bit or
a 1 bit is transferred by the high following the low. If the high is shorter than
the low we have a 0 data bit. If the high is longer than the low we have a 1 data bit.

To make sure the data was transferred correctly, we calculate and compare a data checksum.
If this checksum does not match what was transferred, there was a transmission error and
the data is invalid.

Those short timings pose quite a challenge on a standard operating system like Raspbian.
The operating system is free to suspend the current process any time and schedule
some other process to run. If this happens while communicating with the sensor, we miss
data bits and are unable to read the data. Therefore this implementation currently lacks
some reliability and data readings can throw an exception. If this happens, simply try
again.

Note that the sensor has some constraints about when it can be queried. After power up
it needs at least one second before being ready. And between consecutive readings there
should be at least 5 seconds.

**NOTE: Being interrupted during data transfer because the operating system schedules
some other process is quite a problem. One solution could be to move the data transfer
into kernel space. There we should have the guarantee that we are not interrupted by
other threads. A simple kernel module could provide the data from the sensor as a
character device.**

For source code look for the following files and directories:
* `user-space/raspberrypi/modules`
	* `CDht11.hpp` / `CDht11.cpp`
* `user-space/example04-dht11`
	* `main.cpp`
