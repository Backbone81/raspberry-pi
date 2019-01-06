#include "C433MHzReceiver.hpp"
#include "../CRaspberryPi.hpp"

C433MHzReceiver::C433MHzReceiver(CRaspberryPi& RaspberryPi, int GpioPin)
    : C433MHz(RaspberryPi, GpioPin)
{
    m_Gpio.SetInput(m_GpioPin);
}

void C433MHzReceiver::Receive(void* Data, std::size_t Size)
{
    InternalReceive(Data, Size);
}
