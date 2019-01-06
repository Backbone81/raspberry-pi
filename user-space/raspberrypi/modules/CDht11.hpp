#pragma once

#include <cstdint>

class CRaspberryPi;
class CSystemTimer;
class CGpio;

/*
 * This class provides an abstraction for a DHT11 sensor for measuring temperature and humidity.
 */
class CDht11
{
public:
    /* This is the measurement data returned to the user */
	struct CData
	{
		int Temperature; /* in degrees celcius */
		int Humidity; /* humidity in percent */
	};

public:
    /*
     * No need for copying objects of this class
     */
    CDht11(const CDht11&) = delete;
	CDht11& operator=(const CDht11&) = delete;

public:
    /*
     * Constructor
     * Provide an instance to a raspberry pi object and the GPIO pin for data transmission.
     */
    CDht11(CRaspberryPi& RaspberryPi, int GpioPin);

    /*
     * This method starts a measurement and returns the data from the last measurement.
     * This behaviour is due to how the DHT11 sensor works. Make two consecutive readings
     * to get the most current data. But wait at least 5 seconds between measurements,
     * otherwise the data will be wrong.
     */
	CData Measure();

private:
    /*
     * Signal the sensor that we want to read the data.
     */
	void SendStartSignal();

    /*
     * Wait for the GPIO pin to get low. Returns the microseconds waited until the pin got low.
     */
	int WaitForLow();

    /*
     * Wait for the GPIO pin to get high. Returns the microseconds waited until the pin git high.
     */
    int WaitForHigh();

    /*
     * Extracts the relevant data from the binary raw data.
     */
	CData ProcessData(uint64_t Data);

    /*
     * Calculates the checksum for the data to make sure there was no error during data transmission.
     */
	uint8_t CalculateParity(uint8_t HumidityHigh, uint8_t HumidityLow, uint8_t TemperatureHigh, uint8_t TemperatureLow);

private:
	CSystemTimer& m_SystemTimer;
	CGpio& m_Gpio;
	const int m_GpioPin;

};