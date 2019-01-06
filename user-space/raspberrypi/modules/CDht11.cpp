#include "CDht11.hpp"

#include <chrono>
#include <thread>

#include "../CRaspberryPi.hpp"

CDht11::CDht11(CRaspberryPi& RaspberryPi, int GpioPin)
	: m_SystemTimer(RaspberryPi.SystemTimer()),
	  m_Gpio(RaspberryPi.Gpio()),
	  m_GpioPin(GpioPin)
{
	m_Gpio.SetOutput(m_GpioPin);
	m_Gpio.SetHigh(m_GpioPin);
}

CDht11::CData CDht11::Measure()
{
	uint64_t Data = 0;
	SendStartSignal();
	m_Gpio.SetInput(m_GpioPin);
	try
	{
		WaitForLow();
		WaitForHigh();
		WaitForLow();
		for (int i = 0; i < 40; ++i)
		{
			Data <<= 1;
			int LowTime = WaitForHigh();
			int HighTime = WaitForLow();
			if (LowTime < HighTime)
			{
				Data |= 0x1;
			}
		}
		WaitForHigh();
	}
	catch (...)
	{
		m_Gpio.SetOutput(m_GpioPin);
		m_Gpio.SetHigh(m_GpioPin);
		throw;
	}
	m_Gpio.SetOutput(m_GpioPin);
	m_Gpio.SetHigh(m_GpioPin);
	return ProcessData(Data);
}

int CDht11::WaitForLow()
{
	auto StartTime = m_SystemTimer.GetTicks();
	while (m_Gpio.IsHigh(m_GpioPin))
	{
		if (m_SystemTimer.GetFrequency() < m_SystemTimer.GetTicks() - StartTime)
		{
			throw std::runtime_error("Timeout while waiting for pin to get low.");
		}
	}
	return m_SystemTimer.GetTicks() - StartTime;
}

int CDht11::WaitForHigh()
{
	auto StartTime = m_SystemTimer.GetTicks();
	while (m_Gpio.IsLow(m_GpioPin))
	{
		if (m_SystemTimer.GetFrequency() < m_SystemTimer.GetTicks() - StartTime)
		{
			throw std::runtime_error("Timeout while waiting for pin to get high.");
		}
	}
	return m_SystemTimer.GetTicks() - StartTime;
}

void CDht11::SendStartSignal()
{
	m_Gpio.SetLow(m_GpioPin);
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	m_Gpio.SetHigh(m_GpioPin);
}

CDht11::CData CDht11::ProcessData(uint64_t Data)
{
	uint8_t HumidityHigh = (Data >> 32) & 0xFF;
	uint8_t HumidityLow = (Data >> 24) & 0xFF;
	uint8_t TemperatureHigh = (Data >> 16) & 0xFF;
	uint8_t TemperatureLow = (Data >> 8) & 0xFF;
	uint8_t Parity = Data & 0xFF;
	
	if (Parity != CalculateParity(HumidityHigh, HumidityLow, TemperatureHigh, TemperatureLow))
	{
		throw std::runtime_error("Parity checksum failed.");
	}

	CData Result;
	Result.Temperature = TemperatureHigh;
	Result.Humidity = HumidityHigh;
	return Result;
}

uint8_t CDht11::CalculateParity(uint8_t HumidityHigh, uint8_t HumidityLow, uint8_t TemperatureHigh, uint8_t TemperatureLow)
{
	return static_cast<uint8_t>(HumidityHigh + HumidityLow + TemperatureHigh + TemperatureLow);
}
