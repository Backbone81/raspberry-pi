Part 04: Controlling GPIO ports with C++ on a Raspberry Pi (blinking LED)
=========================================================================
The documentation for the GPIO ports can be found in the
[Peripheral specification](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf)
in chapter 6 page 89.

Controlling the GPIO ports is just a matter of setting or clearing the right
bits of the GPIO registers. As an example we will blink a LED as a simple
proof of concept. The setup looks like this:

![image](images/part%2004%20wiring.png)

I am using a red LED with a forward voltage of 2.0V and a forward current of 20 mA.
Check the datasheet for your own LEDs to make sure you are using the right values
in the calculations to follow.

LEDs always have to be used with a current limiting resistor to prevent them
from burning through. The Rapsberry Pi GPIO pin is providing 3.3V but my LED
only handles up to 2.0V. The difference of 1.3V must be consumed by that resistor.
To calculate the size of the needed resistor, we can use Ohm's law (U = R * I).
Solving for the resistance R gives us R = U / I = 1.3V / 0.02A = 65 Ohm. Therefore
we need a resistor of at least 65 Ohm to prevent the LED from burning. Round
up to the next bigger resistor you have available.

It does not matter if the resistor is on the positive side of the led or the ground
side. As they are built in series they experience the same current no matter
where the resistor is.

Make sure the longer leg of the LED is connected to the GPIO pin and the shorter
leg is connected to ground. Otherwise the LED will not work.

For source code look for the following files and directories:
* `user-space/raspberrypi`
	* `CGpio.hpp` / `CGpio.cpp`
* `user-space/raspberrypi/modules`
	* `CLed.hpp` / `CLed.cpp`
* `user-space/example01-led`
	* `main.cpp`
