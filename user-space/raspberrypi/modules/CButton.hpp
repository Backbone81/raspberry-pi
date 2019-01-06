#pragma once

class CRaspberryPi;
class CGpio;

/*
 * This class provides an abstraction for a simple button. It detects the low state of
 * a GPIO pin and assumes the button is currently released. A high state for a GPIO pin
 * is interpreted as the button being pressed.
 */
class CButton
{
public:
    /*
     * No need for copying objects of this class
     */
    CButton(const CButton&) = delete;
    CButton& operator=(const CButton&) = delete;

public:
    /*
     * Constructor
     * Provide an instance for the raspberry pi and the GPIO pin the button is connected to
     */
    CButton(CRaspberryPi& RaspberryPi, int GpioPin);

    /*
     * Returns true if the button is currently not pressed (the GPIO pin is low).
     */
	bool IsUp() const;

    /*
     * Returns true if the button is currently pressed (the GPIO pin is high).
     */
    bool IsDown() const;

private:
	CGpio& m_Gpio;
	const int m_GpioPin;

};
