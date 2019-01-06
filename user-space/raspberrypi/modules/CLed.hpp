#pragma once

class CRaspberryPi;
class CGpio;

/*
 * This class provides an abstraction for a simple LED. It sets the GPIO pin high to switch the LED on
 * or switches in low to switch the LED off.
 */
class CLed
{
public:
    /*
     * No need for copying objects of this class
     */
    CLed(const CLed&) = delete;
	CLed& operator=(const CLed&) = delete;

public:
    /*
     * Constructor
     * Provide an instance of the raspberry pi and the GPIO pin where the LED is connected to.
     */
	CLed(CRaspberryPi& RaspberryPi, int GpioPin);

    /*
     * Switches the GPIO pin to high to enable the LED
     */
	void On();

    /*
     * Switches the GPIO pin to low to disable the LED
     */
	void Off();

private:
	CGpio& m_Gpio;
	const int m_GpioPin;

};
