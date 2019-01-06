Part 05: Detecting a pressed button with C++ on a Raspberry Pi
==============================================================
To detect a pressed button, we check if a circuit is closed and voltage is provided
to our GPIO pin. The setup looks like this:

![image](images/part%2005%20wiring.png)

The 3.3V is connected to our GPIO pin through the button. If the button is pressed,
the curcuit is closed and the 3.3V connect to the GPIO pin signaling the pressed
button.

But we have a problem while the button is not pressed. A GPIO pin which is not connected
to anything is called a floating pin. Its state is not deterministic and should be
assumed to be random. To bring that pin into a reliable low state, we use a so called
pull down resistor which connects the GPIO pin to ground. As long as the button is not
pressed, the GPIO pin is low by the connection to ground. When the button is pressed,
it is high. I used a pull down resistor with 10K Ohm.

**NOTE: In the [Peripheral specification](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf)
on page 100 there is some documentation about pull up and pull down resistors which are
integrated into the Raspberry Pi. Using those should make using own pull up or pull
down resistors obsolete. For the sake of simplicity this part did not use this feature.**

**NOTE: The example implementation works by polling the current state of the button. This
is bad because without any sleeps the processor will have a CPU core under maximum load
just for checking the state of the button. If we introduce sleeps the CPU core is not
under maximum load but depending the the length of the sleep we could miss a quick button
press because press and release both happen during the sleep. This should best be solved
differently.**

For source code look for the following files and directories:
* `user-space/raspberrypi/modules`
	* `CButton.hpp` / `CButton.cpp`
* `user-space/example02-button`
	* `main.cpp`
