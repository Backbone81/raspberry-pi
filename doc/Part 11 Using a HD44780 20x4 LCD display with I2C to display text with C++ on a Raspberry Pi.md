Part 11: Using a HD44780 20x4 LCD display with I2C to display text with C++ on a Raspberry Pi
=============================================================================================
The last part showed how to use a HD44780 display directly. But that display came with a
I2C adapter which I did not use on purpose. Now I want to use it to save wires I have to connect
to my Raspberry Pi. Again I did not have a datasheet for that adapter. On the plate there
is the name "FC-113" but a search did not turn up any usable datasheet. The next thing was
searching for the name printed on the controller on that chip. This reads "PCF8574T" and a
[datasheet](https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf) could be found.

The idea behind PCF8574T is that with I2C you can set 8 datalines to high and low. The IC
does not have any registers you can access through I2C. Instead every byte you send to that
module will toggle the 8 lines corresponding to each bit. When you read a byte, you get
the information about each line being high or low. That way we can save a ton of cables.

But 8 lines are not enough to use the HD44780 display in 8 bit mode. We would need 8 data bits
and three additional bits for RS, RW and E. Instead with the I2C adapter we have to use
our display in 4 bit mode. This means that each 8 bit command is divided into two 4 bit
transfers with the high 4 bit being transferred first and the low 4 bit being transferred last.

With no datasheet for the FC-113 chip but only for the PCF8574T IC we have no idea about
how the bits from the I2C transfer match to the display pins. For this to find out, I set
up a small testsuite where I sent single active bits and measured with my multimeter which
pins change their voltage. That way I came up with this mapping:

* Bit 0: R/S
* Bit 1: R/W
* Bit 2: E
* Bit 3: Backlight enable
* Bit 4: D4
* Bit 5: D5
* Bit 6: D6
* Bit 7: D7

What is quite interesting is bit 3. It disables/enables the LCD backlight by changing the
backlight power supply.

With that knowledge established we simply enable the 4 bit data bus as first display command.
The write method is split up into two seperate writes with 4 bits each. The rest of the class
stays the same as in part 10.

For source code look for the following files and directories:
* `user-space/raspberrypi/modules`
	* `CHd44780i2c.hpp` / `CHd44780i2c.cpp`
* `user-space/example07-hd44780-i2c`
	* `main.cpp`
