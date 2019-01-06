#include "CHcsr04.hpp"

#include <stdexcept>
#include "../CRaspberryPi.hpp"

CHcsr04::CHcsr04(CRaspberryPi& RaspberryPi, int TriggerGpioPin, int EchoGpioPin)
	: m_Gpio(RaspberryPi.Gpio()),
      m_SystemTimer(RaspberryPi.SystemTimer()),
	  m_TriggerGpioPin(TriggerGpioPin),
      m_EchoGpioPin(EchoGpioPin)
{
    m_Gpio.SetOutput(m_TriggerGpioPin);
    m_Gpio.SetInput(m_EchoGpioPin);
}

float CHcsr04::GetDistance()
{
    SendTriggerSignal();
    return GetEchoLength() * GetSpeedOfSound() / 2.0f;
}

void CHcsr04::SendTriggerSignal()
{
    m_Gpio.SetHigh(m_TriggerGpioPin);
    auto Start = m_SystemTimer.GetTicks();
    while (m_SystemTimer.GetTicks() - Start < 10)
    {
        // wait for 10 microseconds to have elapsed
    }
    m_Gpio.SetLow(m_TriggerGpioPin);
}

void CHcsr04::WaitForEchoHigh()
{
    auto Start = m_SystemTimer.GetTicks();
    while (m_Gpio.IsLow(m_EchoGpioPin))
    {
        if (m_SystemTimer.GetFrequency() < m_SystemTimer.GetTicks() - Start)
        {
            throw std::runtime_error("Timeout while waiting for echo signal to become high.");
        }
    }
}

void CHcsr04::WaitForEchoLow()
{
    auto Start = m_SystemTimer.GetTicks();
    while (m_Gpio.IsHigh(m_EchoGpioPin))
    {
        if (m_SystemTimer.GetFrequency() < m_SystemTimer.GetTicks() - Start)
        {
            throw std::runtime_error("Timeout while waiting for echo signal to become low.");
        }
    }
}

float CHcsr04::GetEchoLength()
{
    WaitForEchoHigh();
    auto Start = m_SystemTimer.GetTicks();
    WaitForEchoLow();
    auto End = m_SystemTimer.GetTicks();
    return static_cast<float>(End - Start) / static_cast<float>(m_SystemTimer.GetFrequency());
}

float CHcsr04::GetSpeedOfSound()
{
    // the speed of sound for dry air with a temperature of 20°C in meters per second
    return 343.2f;
}
