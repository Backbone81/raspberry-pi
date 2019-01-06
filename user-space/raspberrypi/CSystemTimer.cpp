#include "CSystemTimer.hpp"

CSystemTimer::CSystemTimer(void* Base)
    : m_Register(reinterpret_cast<uint32_t*>(Base)),
      m_Counter(m_Register + 1)
{

}

uint32_t CSystemTimer::GetTicks() const
{
    return *m_Counter;
}

uint32_t CSystemTimer::GetFrequency() const
{
    return 1000000;
}
