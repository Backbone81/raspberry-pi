#include "CRaspberryPi.hpp"

CRaspberryPi::CRaspberryPi()
    : m_SystemTimerMemory(m_PhysicalMemory.GetHandle(), SYSTEM_TIMER_BASE, SYSTEM_TIMER_SIZE),
      m_SystemTimer(m_SystemTimerMemory.GetHandle()),
      m_GpioMemory(m_PhysicalMemory.GetHandle(), GPIO_BASE, GPIO_SIZE),
      m_Gpio(m_GpioMemory.GetHandle()),
      m_Bsc0Memory(m_PhysicalMemory.GetHandle(), BSC0_BASE, BSC_SIZE),
      m_Bsc0(m_Bsc0Memory.GetHandle()),
      m_Bsc1Memory(m_PhysicalMemory.GetHandle(), BSC1_BASE, BSC_SIZE),
      m_Bsc1(m_Bsc1Memory.GetHandle())
{
}

CRaspberryPi::~CRaspberryPi()
{
}

CSystemTimer& CRaspberryPi::SystemTimer()
{
    return m_SystemTimer;
}

CGpio& CRaspberryPi::Gpio()
{
    return m_Gpio;
}

CBsc& CRaspberryPi::Bsc0()
{
    return m_Bsc0;
}

CBsc& CRaspberryPi::Bsc1()
{
    return m_Bsc1;
}
