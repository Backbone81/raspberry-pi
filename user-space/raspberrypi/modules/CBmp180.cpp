#include "CBmp180.hpp"

#include <stdexcept>
#include <chrono>
#include <thread>
#include "../CRaspberryPi.hpp"

CBmp180::CBmp180(CRaspberryPi& RaspberryPi, int SdaGpioPin, int SclGpioPin)
	: m_Gpio(RaspberryPi.Gpio()),
      m_Bsc(RaspberryPi.Bsc1()),
	  m_SdaGpioPin(SdaGpioPin),
      m_SclGpioPin(SclGpioPin)
{
    m_Gpio.SetAlternateFunction(0, m_SdaGpioPin);
    m_Gpio.SetAlternateFunction(0, m_SclGpioPin);
    ValidateCommunication();
    GetCalibrationData();
}

CBmp180::CData CBmp180::Measure() const
{
    CData Result;

    // read raw temperature and pressure data
    uint16_t UT = GetUncompensatedTemperature();
    uint16_t UP = GetUncompensatedPressure();

    // calculate real temperature data with the help of the calibration data
    // this is ugly as hell but taken from the BMP180 datasheet
    // how is anyone able to maintain or debug such code?
    int X1 = (UT - m_Calibration.AC6) * m_Calibration.AC5 / (0x1 << 15);
    int X2 = m_Calibration.MC * (0x1 << 11) / (X1 + m_Calibration.MD);
    int B5 = X1 + X2;
    int T = (B5 + 8) / (0x1 << 4);
    Result.Temperature = static_cast<float>(T) / 10.0f;

    // calculate real pressure data with the help of the calibration data
    // this is ugly as hell but taken from the BMP180 datasheet
    // how is anyone able to maintain or debug such code?
    int B6 = B5 - 4000;
    X1 = (m_Calibration.B2 * (B6 * B6 / (0x1 << 12))) / (0x1 << 11);
    X2 = m_Calibration.AC2 * B6 / (0x1 << 11);
    int X3 = X1 + X2;
    int B3 = ((m_Calibration.AC1 * 4 + X3) + 2) / 4;
    X1 = m_Calibration.AC3 * B6 / (0x1 << 13);
    X2 = (m_Calibration.B1 * (B6 * B6 / (0x1 << 12))) / (0x1 << 16);
    X3 = ((X1 + X2) + 2) / (0x1 << 2);
    unsigned int B4 = m_Calibration.AC4 * static_cast<unsigned int>(X3 + 32768) / (0x1 << 15);
    int B7 = (UP - B3) * 50000;
    int p = (B7 < static_cast<int>(0x80000000)) ? ((B7 * 2) / B4) : (B7 / B4) * 2;
    X1 = (p / (0x1 << 8)) * (p / (0x1 << 8));
    X1 = (X1 * 3038) / (0x1 << 16);
    X2 = (-7357 * p) / (0x1 << 16);
    Result.Pressure = static_cast<float>(p + (X1 + X2 + 3791) / (0x1 << 4));

    return Result;
}

uint16_t CBmp180::GetUncompensatedTemperature() const
{
    // request temperature data to be collected
    uint8_t Command = 0x2E;
    m_Bsc.Write(MODULE_ADDRESS, REG_CTRL_MEAS_ADDRESS, &Command, sizeof(Command));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // make sure the data collection is finished
    m_Bsc.Read(MODULE_ADDRESS, REG_CTRL_MEAS_ADDRESS, &Command, sizeof(Command));
    if (Command & (0x1 << 5))
    {
        throw std::runtime_error("No temperature data after waiting for it.");
    }

    // read raw temperature data
    uint16_t UT = 0;
    m_Bsc.Read(MODULE_ADDRESS, REG_MSB_ADDRESS, &UT, sizeof(UT));
    SwitchEndian(UT);
    return UT;
}

uint16_t CBmp180::GetUncompensatedPressure() const
{
    // request pressure data to be collected
    uint8_t Command = 0x34;
    m_Bsc.Write(MODULE_ADDRESS, REG_CTRL_MEAS_ADDRESS, &Command, sizeof(Command));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // make sure the data collection is finished
    m_Bsc.Read(MODULE_ADDRESS, REG_CTRL_MEAS_ADDRESS, &Command, sizeof(Command));
    if (Command & (0x1 << 5))
    {
        throw std::runtime_error("No pressure data after waiting for it.");
    }

    // read raw pressure data
    uint16_t UP = 0;
    m_Bsc.Read(MODULE_ADDRESS, REG_MSB_ADDRESS, &UP, sizeof(UP));
    SwitchEndian(UP);
    return UP;
}

void CBmp180::GetCalibrationData()
{
    if (sizeof(m_Calibration) != 11 * sizeof(uint16_t))
    {
        throw std::runtime_error("Calibration data structure does not meet expected size.");
    }
    m_Bsc.Read(MODULE_ADDRESS, REG_CALIB_ADDRESS, &m_Calibration, sizeof(m_Calibration));

    SwitchEndian(m_Calibration.AC1);
    SwitchEndian(m_Calibration.AC2);
    SwitchEndian(m_Calibration.AC3);
    SwitchEndian(m_Calibration.AC4);
    SwitchEndian(m_Calibration.AC5);
    SwitchEndian(m_Calibration.AC6);
    SwitchEndian(m_Calibration.B1);
    SwitchEndian(m_Calibration.B2);
    SwitchEndian(m_Calibration.MB);
    SwitchEndian(m_Calibration.MC);
    SwitchEndian(m_Calibration.MD);
}

void CBmp180::ValidateCommunication() const
{
    uint8_t Id = 0;
    m_Bsc.Read(MODULE_ADDRESS, REG_ID_ADDRESS, &Id, sizeof(Id));
    if (Id != REFERENCE_ID)
    {
        throw std::runtime_error("Communication with the BMP180 module through IC2 failed.");
    }
}

void CBmp180::SwitchEndian(uint16_t& Value) const
{
    Value = static_cast<uint16_t>(((Value & 0x00FF) << 8) | ((Value & 0xFF00) >> 8));
}

void CBmp180::SwitchEndian(int16_t& Value) const
{
    Value = static_cast<int16_t>(((Value & 0x00FF) << 8) | ((Value & 0xFF00) >> 8));
}
