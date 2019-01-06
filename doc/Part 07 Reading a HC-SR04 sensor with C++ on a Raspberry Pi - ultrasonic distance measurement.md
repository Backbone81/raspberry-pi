Part 07: Reading a HC-SR04 sensor with C++ on a Raspberry Pi - ultrasonic distance measurement
==============================================================================================
I have bought the HC-SR04 sensor from [AZ-Delivery](https://www.az-delivery.de) where they also provide the
[datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/HC-SR04_Ultraschallmodule_Datenblatt.pdf?8650529508855745030).

HC-SR04 is an ultrasonic distance measurement sensor. It has a trigger connector which must be set
to high for at least 10 microseconds. Afterwards it sends multiple ultrasonic pulses to measure the
distance and sets the echo connector high for as long as the sound waves needed to travel. This
time must be measured and the half of the distance is the distance to the next object.

The setup looks like this:

![image](images/part%2007%20wiring.png)

Note that the module uses 5V whereas our GPIO pins only work with 3.3V. The datasheet is not clear
about the voltage of the echo pin. So we assume the sensor provides the 5V it gets directly to
the echo pin. To avoid burning our GPIO pin, we need a resistor to catch the excess 1.7V. With
Ohm's law I use R = U / I = 1.7V / 0.005A = 340 Ohm. The 5mA are chosen arbitrarily as our
GPIO pins do not sustain high current. So I want to limit it to 5mA. In the wiring I rounded
the 340 Ohm to a 330 Ohm resistor. Also note that we use a pull down resistor of 10k Ohm to make
sure our echo GPIO port has a defined state when no voltage is provided by the echo pin.

Note that our helper methods which are waiting for the echo state to become high or low have
a timeout of one second. If the timeout is hit, they throw an exception. Otherwise we could
be stuck in an endless loop.

The distance measurement is dependent on the speed of sound. We are using a fixed speed of 343.2
meters per second for dry air with a temperature of 20°C.

**NOTE: Waiting for the echo signal to become high or low is a busy wait which is quite inefficient.
If the timeout is hit, the busy wait will cause one CPU core to have maximum load for one second.
This should be changed to something more efficient.***

For source code look for the following files and directories:
* `user-space/raspberrypi/modules`
	* `CHcsr04.hpp` / `CHcsr04.cpp`
* `user-space/example03-hcsr04`
	* `main.cpp`
