Part 09: Reading a BMP180 sensor with C++ through I2C on a Raspberry Pi - temperature and pressure
==================================================================================================
I have bought the DHT11 sensor from [AZ-Delivery](https://www.az-delivery.de) where they also provide the
[datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/GY-68_BMP180_Barometrischer_Sensor_Luftdruck_Modul_fur_Arduino_und_Raspberry_Pi_Datenblatt.pdf?15836792964504220844).

BMP180 is a sensor which provides temperature and air pressure data. Communication is done through
the I2C interface.

The setup looks like this:

![image](images/part%2009%20wiring.png)

The connections are straight forward. The one thing to note are the two pull up resistors with 5.1k Ohm
which are need for the SDA and the SCL line. Modules communicating over I2C are only able to pull the
voltage down, but not up. Therefore we need pull up resistors which pull the voltage high, when the
module lets go. Note that I2C is a bus which supports many modules in series. You only need one set
of pull up resistors for the whole bus - not for every module.

For the GPIO ports to be used as SDA and SCL lines, you have to activate the alternate GPIO functions.
See the [Peripheral specification](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf)
chapter 6.2 page 102.

Good information about how to control I2C communication is hard to come by. The component of the
Raspberry Pi we need to control is the Broadcom Serial Controller (BSC). Its documentation can be found
in the [Peripheral specification](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf)
in chapter 3 on page 28. Helpful were also the BMP180 datasheet, [Low Level Programming of the Raspberry Pi in C](http://pieter-jan.com)
and [Using the I2C Bus](https://robot-electronics.co.uk/i2c-tutorial).

The BSC provides a control register for telling the BSC what we want to do, a status register
which we can query about the current transmission state, a data length register which we use
to tell the BSC how many bytes of data we want to read or write, a slave address register
which tells the BSC which module we want to talk to and a data FIFO register which we can
read or write single bytes to.

Writing data to a module works like this:
* set the slave address of the module we want to write to
* clear the status register so we are clean for the next transaction
* set the data length to the number of bytes we want to write
* set the control register to start a write transaction
* write data to the data FIFO register
* wait until the status register signals the end of the transaction

When writing data to a module, we usually want to write to a specific module register. The address
of the target module register is always the first byte. So if you want to write one byte into
a module register, you have to write two bytes. The first byte is the register address and the
second byte is the data you want to write into that register. While writing to the data FIFO,
you also have to be sure the BSC is ready to process more data. Always query the status register
for free space in the FIFO before writing to the data FIFO. Data written to the data FIFO while
it has no space left will just silently get lost.

Note that the order in which you clear the status register, set the data length and start the
transaction matters. When you first start the transaction and then clear the status register,
the communication will work most of the time but fail sometimes. There was nothing about this
issue in the datasheets or the BSC documentation. But the issue arose and was solved by peeking
into other implementations and trying to switch the order.

Reading data from a module works like this:
* do a complete write transaction with the register address for the register you want to read (see above)
* set the slave address of the module we want to read from
* clear the status register so we are clean for the next transaction
* set the data length to the number of bytes we want to read
* set the control register to start a read transaction
* read data from the data FIFO register
* wait until the status register signals the end of the transaction

While reading from the data FIFO, you have to be sure the BSC is ready to deliver more data.
Always query the status register for waiting data in the FIFO before reading data from the FIFO.
Data read while the FIFO has no data available will silently return garbage data.

Now back to the BMP180 sensor we want to use. Through I2C we can tell the sensor to do a temperature
or pressure calculation. Both needs some time to complete (around 5ms). Afterwards we can read the
raw data from some module register. Note that the raw temperature or pressure data is not usable right
away. The BMP180 sensor has some calibration data which needs to be used to turn the raw data into
the real data. The calibration data is fixed and can be read at initialization time. Afterward
it can be reused for each measurement. Note that the pressure calculation needs the temperature
data. Because of this the current implementation always read temperature and pressure data and returns
both results. If you were only interested in the temperature, the implementation could be modified
to be faster by skipping the pressure data.

One important thing to keep in mind is the endianness of the data returned vom BMP180. That data
is big endian data which means the most significant byte comes first. Raspbian works with a little
endian system which means the least significant byte comes first. This means for data which is bigger
than one byte we have to switch the byte order for us to have meaningful results.

For source code look for the following files and directories:
* `user-space/raspberrypi`
	* `CBsc.hpp` / `CBsc.cpp`
* `user-space/raspberrypi/modules`
	* `CBmp180.hpp` / `CBmp180.cpp`
* `user-space/example05-bmp180`
	* `main.cpp`
