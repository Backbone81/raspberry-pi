Part 10: Using a HD44780 20x4 LCD display to display text with C++ on a Raspberry Pi
====================================================================================
I have bought the HD44780 display from [AZ-Delivery](https://www.az-delivery.de) but
they did not provide a datasheet for it. Looking at the back of the display shows
2004A as a label on the board. Searching for that name reveals a
[datasheet](https://www.beta-estore.com/download/rk/RK-10290_410.pdf). In chapter 1.1
there is a control IC named SPLC780D which seems to be HD44780 compliant with
its own [datasheet](https://www.newhavendisplay.com/app_notes/SPLC780D.pdf).

This display is often used in conjunction with an I2C adapter for making it easier
to use with less connections. This part of the documentation will use the LCD
display directly without an I2C adapter for the sake of learning how those displays
work.

The first problem is the voltage the display is working with. It needs 5V and will
output a significant portion of this when reading data from the display. This means
that we have a potentially dangerous situation for our Raspberry Pi, because the GPIO
pins are only designed for 3.3V. To solve this issue, I am using three logic level
converters to change between 3.3V on the Rapsberry Pi side and 5V on the display side.

The second problem is the V0 pin of the display. It is responsible for the contrast
and how solid the single pixels appear. If the voltage is too high, the text will get
invisible. If the voltage is too low, all pixels will get solid wether they should be
or not. The datasheet does not provide a specific voltage needed. Therefore we use
a potentiometer to fine tune the voltage by hand. The potentiometer has three legs.
The middle leg is connected to V0, one of the remaining legs is connected to 5V
and the last leg ist connected to ground.

The setup looks like this:

![image](images/part%2010%20wiring.png)

All the cables make quite a mess. We could have saved 4 wires if we used the 4 bit mode
of the display, but there are still enough cables around.

Sending instructions to the display is done with the 8 data bus pins, the enable pin,
RS and R/W. It is important to note that while transferring 8 bits to the display,
there are some minimum timing requirements that have to be met (see datasheet). It is
especially important that the data pins still hold their voltage while the enable
signal is put down. Because the timing is within nanoseconds range, we have no real
possibility to measure such times. Therefore we round up to 2us, because the timing
requirements are all minimum times. Going beyond will not do any harm.

After having sent an instruction, there is a time the display will need to process that
instruction. The datasheet lists needed processing times for all instructions. Because
the longest instruction needs 2.16ms we are using a general wait of 3ms for each
instruction. When sending data to the display, the longest processing time is
53us which we round up to 1ms.

For source code look for the following files and directories:
* `user-space/raspberrypi/modules`
	* `CHd44780.hpp` / `CHd44780.cpp`
* `user-space/example06-hd44780`
	* `main.cpp`

