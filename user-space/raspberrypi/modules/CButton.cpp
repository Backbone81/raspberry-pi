#include "CButton.hpp"

#include "../CRaspberryPi.hpp"

CButton::CButton(CRaspberryPi& RaspberryPi, int GpioPin)
	: m_Gpio(RaspberryPi.Gpio()),
	  m_GpioPin(GpioPin)
{
	m_Gpio.SetInput(m_GpioPin);
}

bool CButton::IsUp() const
{
	return m_Gpio.IsLow(m_GpioPin);
}

bool CButton::IsDown() const
{
    return !IsUp();
}
