Part 15: Optimized reading of a DHT11 sensor with C as a linux device driver on a Raspberry Pi - temperature and humidity
=========================================================================================================================
In "Part 13: Reading a DHT11 sensor with C as a linux device driver on a Raspberry Pi - temperature and humidity" we
learned how to write a linux device driver to avoid being suspended by the operating system while communicating with
the DHT11 sensor. In "Part 14: Detecting a pressed button with C as a linux device driver on a Raspberry Pi" we saw
how hardware interrupts work in device drivers and how they can be used to avoid busy waiting on hardware changes.
I will use both techniques in this part to create a device driver which uses hardware interrupts for communicating
with the DHT11 sensor.

The basic idea is that we will have an interrupt service routine triggered each time our GPIO data pin changes from
high to low or low to high. Each time we note the timestamp of that transition with the help of the system timer
which provides 1 MHz resolution. When we detect a transition from high to low, we have found the end of a single bit
as defined in the DHT11 datasheet. On the end of a single bit, we compare the transistion times to determine if that
bit was a 0 bit or a 1 bit. Those bits are shifted into a 64 bit buffer. We count the bits we receive since the start
of our transmission and stop when we have received the last data bit. The data of the 64 bit buffer is then split
into temperature, humidity and checksum.

In part 13 we made the sensor measurement on each file open event. This would lead to callers being blocked for
2 seconds or longer if there were more open events than one per 2 seconds. This is bad for users of our driver.
Therefore we will change this behaviour in this part. The last measured data will always be stored in a central
buffer. On file open events this data is copied into an individual data buffer for the file open event which enables
arbitrary readers to access our data without any significant delay. Access to the central buffer must then be protected
by a spinlock to avoid corrupted data by race conditions. The data updates are then done through a kernel timer which
will update the central buffer in regular intervals.

Because the output pin of part 14 did work really well for debugging hardware interrupts, we have a debug pin which
we can optinally enable for signaling what the interrupts are detecting on the data pin. That way we can use an
oscilloscope to debug some problems with interrupt handling.

**NOTE: One problem which is still not solved is the 19 millisecond delay we need for our start signal. Currently
this is done by busy waiting. This is not good because it uses resources which could be used differently. Other
solutions have drawbacks with their accuracy. A seperate timer will be way off and cause issues with the sensor not
responding.**

For source code look for the following files and directories:
* `kernel-space\example03-dht11-int`
	* `README.md`
