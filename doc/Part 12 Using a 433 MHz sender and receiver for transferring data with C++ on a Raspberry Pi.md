Part 12: Using a 433 MHz sender and receiver for transferring data with C++ on a Raspberry Pi
=============================================================================================
I have bought the 433 MHz sender and receiver from [AZ-Delivery](https://www.az-delivery.de)
where they also provide the [datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/433_MHz_Funk_-_Sende_und_Empfanger_Modul_Datenblatt.pdf?8261930082626240021).
But the problem is that the datasheet does not provide any information about how these
modules work and how they are intended to be used. Beacuse of this I had to do some experimentation
to find out how they work.

At first I created a small testcircuit where i could trigger the sender with a button and the
receiver would light a LED with the data it received. What I was observing was the LED flickering
with a random pattern when the button was not pressed. This must be the background noise on the
433 MHz band. When I pressed the button, the LED lid up brightly but only for a short amount of
time (less than a second). After that the random flickering started again. When I released the
button, the LED went dark for a short amount of time (less than a second) and then again started
the random flickering. This hints for the receiver to automaticly adjusting its sensitivity
to the data sent on the 433 MHz frequency. For further investigations I was using an oscilloscope
to get reliable measurements for my first observations.

At first I measured the sensitivity adjustment when starting to send data:

![image](images/part%2012%20permanent%20high.png)

The upper yellow line is the data pin on the sender changing from low to high in the middle
of the screen. The lower cyan line is the data pin on the receiver. We can see the random noise
on the left side before the sender starts sending. After starting to send, the noise is gone for
about 100ms and returns afterwards.

Next I measured the sensitivity adjustment when stopping the data transmission:

![image](images/part%2012%20permanent%20low.png)

Again the upper yellow line is the data pin on the sender changing from high to low in the
middle of the screen. The lower cyan line is the data pin on the receiver. We can see some random
noise on the left side before the sender stops sending. After having stopped sending, the noise is
gone for about 150ms and returns afterwards.

This means we cannot use low for a bit with a value of 0 and high for a bit with a value of 1.
When we send data with lots of 0 bits or lots of 1 bits, the receiver would adjust its sensitivity
to that level and we cannot reliably transfer significant amounts of data. Instead we could use the
length of the high and low signals to transfer bit values. To test this idea, I took a look
at my car key which is sending on 433 MHz as well:

![image](images/part%2012%20car%20key.png)

The yellow line is the data pin on the receiver. We can see high and low lines which have differing
length. The short lines are about 250us long which matches with the data transfer rate of the datasheet.
The datasheet specifies a transfer rate of 4 KB/s which means 1 second / 4000 = 250us. So the idea of
transferring the data alternating on the high and low of the signal seems to be the way to go.

Next I tried that approach with alternating highs and lows:

![image](images/part%2012%20equal%20high%20and%20low.png)

The upper yellow line is the data pin on the sender switching between high and low every 250us.
The lower cyan line is the data pin on the receiver. We can see the reiceiver following the sender
with a small delay and great reliability. While alternating between high and low in this way the
receiver is not adjusting its sensitivity to any level which would interfere with our data transmission.

![image](images/part%2012%20long%20high.png)

In that example I was using a high time which was double the low time. We can see that it scales
well on the receiving side.

![image](images/part%2012%20long%20low.png)

And last a longer low which does work fine as well.

Another important aspect is using an antenna for sender and receiver. The modules are delivered without
antenna. But they provide connection holes for soldering in an antenna. This has significant impact
on the signal quality.

![image](images/part%2012%20without%20antenna.png)

The upper yellow line is just a helping trigger high to catch the start of the sender. The middle
cyan line is the data pin on the sender. The lower pink line is the data pin of the receiver. Sender
and receiver are about 20cm apart without any antenna attached to them. We can clearly see that the
receiver does not follow the sender very well. It shows varying timings and even spikes which should
not be there.

![image](images/part%2012%20with%20antenna.png)

The same setup as before but with 17cm antennas soldered to both the sender and the receiver. The timing
is now much better and the spikes are gone.

With all that knowledge I gained through measuring and experimenting I can now use the 433 MHz sender
and receiver for reliable data transmission. Bits with a value of 0 have a length of 250us and bits
with a value of 1 have a length of 500us. Bits are transmitted alternating on high and low. For the
receiver to detect our sender out of the background noise, I am using a magic byte which the sender
always sends before transmitting the data payload. The receiver listens for that magic byte and only
starts processing the data when that byte was detected. After having sent the data payload I send
a one byte checksum which enables the receiver to check if the data was correctly received. That way
I can reliably transfer data with a constant data rate between 250 bytes/s and 500 bytes/s depending
on the bits being transferred.

The distance of the 433 MHz sender and receiver is quite limited. The datasheet states 20 to 200 meters
but this is probably only without obstructions. 5 meters distance with one or two brick-built walls
is already enough to screw the signal so far that the data transmission does not work reliably any
more.

**NOTE: Being interrupted during data transfer because the operating system schedules
some other process is quite a problem. One solution could be to move the data transfer
into kernel space. There we should have the guarantee that we are not interrupted by
other threads. A simple kernel module could provide the data from the sensor as a
character device.***

For source code look for the following files and directories:
* `user-space/raspberrypi/modules`
	* `C433MHz.hpp` / `C433MHz.cpp`
	* `C433MHzReceiver.hpp` / `C433MHzReceiver.cpp`
	* `C433MHzSender.hpp` / `C433MHzSender.cpp`
* `user-space/example08-433mhz-sender`
	* `main.cpp`
* `user-space/example09-433mhz-receiver`
	* `main.cpp`
