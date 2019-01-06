#pragma once

class CRaspberryPi;
class CGpio;
class CSystemTimer;

/*
 * This class provides an abstraction for a HC-SR04 sensor for measuring distances with ultrasonic sound waves.
 */
class CHcsr04
{
public:
    /*
     * No need for copying objects of this class
     */
    CHcsr04(const CHcsr04&) = delete;
    CHcsr04& operator=(const CHcsr04&) = delete;

public:
    /*
     * Constructor
     * Provide an instance of the raspberry pi and the GPIO pins for triggering a measurement and the echo.
     */
    CHcsr04(CRaspberryPi& RaspberryPi, int TriggerGpioPin, int EchoGpioPin);

    /*
     * Returns the distance in meters
     */
	float GetDistance();

protected:
    /*
     * Send the required trigger signal to start a measurement.
     */
    void SendTriggerSignal();

    /*
     * Blocks until the echo pin is high.
     * If no high state is detected after one second, this method throws an exception.
     */
    void WaitForEchoHigh();

    /*
     * Blocks until the echo pin is low.
     * If no low state is detected after one second, this method throws an exception.
     */
    void WaitForEchoLow();

    /*
     * Returns the time in seconds for the length of the echo signal
     */
    float GetEchoLength();

    /*
     * Returns the speed of sound in meters per second.
     */
    float GetSpeedOfSound();

private:
	CGpio& m_Gpio;
    CSystemTimer& m_SystemTimer;
    const int m_TriggerGpioPin;
    const int m_EchoGpioPin;

};
