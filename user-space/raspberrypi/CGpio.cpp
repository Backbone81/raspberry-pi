#include "CGpio.hpp"

#include <stdexcept>

CGpio::CGpio(void* Base)
    : m_Register(reinterpret_cast<uint32_t*>(Base)),
      m_FunctionSelect(m_Register),
      m_PinOutputSet(m_Register + 7),
      m_PinOutputClear(m_Register + 10),
      m_PinLevel(m_Register + 13)
{
}

void CGpio::SetInput(int Pin)
{
    int RegisterIndex = Pin / 10;
    int BitOffset = (Pin % 10) * 3;
    m_FunctionSelect[RegisterIndex] &= ~(0x7 << BitOffset);
}

void CGpio::SetOutput(int Pin)
{
    int RegisterIndex = Pin / 10;
    int BitOffset = (Pin % 10) * 3;
    m_FunctionSelect[RegisterIndex] = (m_FunctionSelect[RegisterIndex] & ~(0x7 << BitOffset)) | (0x1 << BitOffset);
}

void CGpio::SetHigh(int Pin)
{
    int RegisterIndex = Pin / 32;
    int BitOffset = Pin % 32;
    m_PinOutputSet[RegisterIndex] = (0x1 << BitOffset);
}

void CGpio::SetLow(int Pin)
{
    int RegisterIndex = Pin / 32;
    int BitOffset = Pin % 32;
    m_PinOutputClear[RegisterIndex] = (0x1 << BitOffset);
}

void CGpio::Set(int Pin, bool High)
{
    if (High)
    {
        SetHigh(Pin);
    }
    else
    {
        SetLow(Pin);
    }
}

bool CGpio::IsHigh(int Pin) const
{
    int RegisterIndex = Pin / 32;
    int BitOffset = Pin % 32;
    return m_PinLevel[RegisterIndex] & (0x1 << BitOffset);
}

bool CGpio::IsLow(int Pin) const
{
    return !IsHigh(Pin);
}

void CGpio::SetAlternateFunction(int FunctionId, int Pin)
{
    const int FunctionCode[] = { 0x4,  0x5, 0x6, 0x7, 0x3, 0x2 };
    if (FunctionId < 0 || 5 < FunctionId)
    {
        throw std::runtime_error("Unknown GPIO alternate function id.");
    }
    int RegisterIndex = Pin / 10;
    int BitOffset = (Pin % 10) * 3;
    m_FunctionSelect[RegisterIndex] = (m_FunctionSelect[RegisterIndex] & ~(0x7 << BitOffset)) | (FunctionCode[FunctionId] << BitOffset);
}
