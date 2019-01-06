#include "CLed.hpp"

#include "../CRaspberryPi.hpp"

CLed::CLed(CRaspberryPi& RaspberryPi, int GpioPin)
	: m_Gpio(RaspberryPi.Gpio()),
	  m_GpioPin(GpioPin)
{
	m_Gpio.SetOutput(m_GpioPin);
}

void CLed::On()
{
	m_Gpio.SetHigh(m_GpioPin);
}

void CLed::Off()
{
	m_Gpio.SetLow(m_GpioPin);
}
