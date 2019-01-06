#include "C433MHzSender.hpp"
#include "../CRaspberryPi.hpp"

C433MHzSender::C433MHzSender(CRaspberryPi& RaspberryPi, int GpioPin)
    : C433MHz(RaspberryPi, GpioPin)
{
    m_Gpio.SetOutput(m_GpioPin);
    m_Gpio.SetLow(m_GpioPin);
}

void C433MHzSender::Send(const void* Data, std::size_t Size)
{
    InternalSend(Data, Size);
}
