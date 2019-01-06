#pragma once

#include <cstdint>

class CRaspberryPi;
class CGpio;
class CBsc;

/*
 * This class provides an abstraction for a BMP180 barometric sensor.
 */
class CBmp180
{
public:
    /* The datasheet specifies the read address as 0xEF and write address as 0xEE.
     * Read and write operations are signaled by a specific bit. When we drop this
     * bit, the address becomes 0x77 which matches with what 'i2cdetect -y 1'
     * prints out.
     */
    const uint8_t MODULE_ADDRESS = 0x77;
    const uint8_t REG_ID_ADDRESS = 0xD0;
    const uint8_t REFERENCE_ID = 0x55;
    const uint8_t REG_CTRL_MEAS_ADDRESS = 0xF4;
    const uint8_t REG_MSB_ADDRESS = 0xF6;
    const uint8_t REG_CALIB_ADDRESS = 0xAA;

    /*
     * The temperature and pressure data we read from the sensor is uncompensated data.
     * We need to use the calibration coefficients to turn the uncompensated data into
     * real data. This structure holds the calibration coefficients.
     */
    struct CCalibrationCoefficients
    {
        int16_t AC1;
        int16_t AC2;
        int16_t AC3;
        uint16_t AC4;
        uint16_t AC5;
        uint16_t AC6;
        int16_t B1;
        int16_t B2;
        int16_t MB;
        int16_t MC;
        int16_t MD;
    };

    /*
     * This is a helper structure which holds the temperature in °C and the pressure in pa.
     */
    struct CData
    {
        float Temperature;
        float Pressure;
    };

public:
    /*
     * No need for copying objects of this class
     */
    CBmp180(const CBmp180&) = delete;
    CBmp180& operator=(const CBmp180&) = delete;

public:
    /*
     * Constructor
     * Provide an instance of the raspberry pi and the GPIO pins for the I2C connection.
     * Tests communication with the sensor and reads calibration data.
     */
    CBmp180(CRaspberryPi& RaspberryPi, int SdaGpioPin, int SclGpioPin);

    /*
     * Measures temperature and pressure and returns it as a result. Seperating
     * temperature and pressure data would be difficult because for calculating
     * the pressure we need parts of the temperature calculation. Therefore we
     * always return both.
     */
    CData Measure() const;

    /*
     * Checks if the I2C communication with the module works. This is done by reading the id
     * register of the module which should deliver a fixed value according to the datasheet.
     * If communication failed, this method will raise an exception.
     */
    void ValidateCommunication() const;

private:
    /*
     * Reads calibration data from the sensor. This is only needed once because the calibration
     * does not change.
     */
    void GetCalibrationData();

    /*
     * Returns the raw temperature from the sensor which must then be processed with the calibration data.
     */
    uint16_t GetUncompensatedTemperature() const;

    /*
     * Returns the raw pressure from the sensor which must then be processed with the calibration data.
     */
    uint16_t GetUncompensatedPressure() const;

    /*
     * Helper method for changing big endian to little endian or vice versa.
     * The data returnes from the sensor is big endian but Raspbian is working in little endian.
     */
    void SwitchEndian(uint16_t& Value) const;

    /*
     * Helper method for changing big endian to little endian or vice versa.
     * The data returnes from the sensor is big endian but Raspbian is working in little endian.
     */
    void SwitchEndian(int16_t& Value) const;

private:
	CGpio& m_Gpio;
    CBsc& m_Bsc;
    const int m_SdaGpioPin;
    const int m_SclGpioPin;
    CCalibrationCoefficients m_Calibration;

};
