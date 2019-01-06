#include "C433MHz.hpp"

#include <stdexcept>
#include <chrono>
#include <thread>
#include "../CRaspberryPi.hpp"

C433MHz::C433MHz(CRaspberryPi& RaspberryPi, int GpioPin)
    : m_Gpio(RaspberryPi.Gpio()),
    m_SystemTimer(RaspberryPi.SystemTimer()),
    m_GpioPin(GpioPin)
{
}

void C433MHz::InternalSend(const void* Data, std::size_t Size)
{
    const uint8_t* TypedData = reinterpret_cast<const uint8_t*>(Data);
    uint8_t Checksum = CalculateChecksum(Data, Size);
    StartSend();
    SendByte(MAGIC_BYTE);
    for (std::size_t iByte = 0; iByte < Size; ++iByte, ++TypedData)
    {
        SendByte(*TypedData);
    }
    SendByte(Checksum);
    EndSend();
}

void C433MHz::InternalReceive(void* Data, std::size_t Size)
{
    uint8_t* TypedData = reinterpret_cast<uint8_t*>(Data);
    WaitForMagicByte();
    for (std::size_t iByte = 0; iByte < Size; ++iByte, ++TypedData)
    {
        *TypedData = ReceiveByte();
    }
    uint8_t Checksum = ReceiveByte();
    if (Checksum != CalculateChecksum(Data, Size))
    {
        throw std::runtime_error("Invalid data checksum.");
    }
}

uint8_t C433MHz::CalculateChecksum(const void* Data, std::size_t Size)
{
    uint8_t Checksum = 0;
    const uint8_t* TypedData = reinterpret_cast<const uint8_t*>(Data);
    for (std::size_t iByte = 0; iByte < Size; ++iByte, ++TypedData)
    {
        Checksum = (Checksum + *TypedData) & 0xFF;
    }
    return Checksum;
}

void C433MHz::StartSend()
{
    // we initialize sending our data with a high bit to get the receivers sensitivity adjusted
    m_DataOnHigh = true;
    m_Gpio.Set(m_GpioPin, m_DataOnHigh);
    Sleep(3 * 1000); // wait for the receivers sensitivity to be adjusted
}

void C433MHz::SendByte(uint8_t Data)
{
    // send the highest order bit first so we can read it in first and shift it to the left when reading the magic byte
    for (int iBit = 7; iBit >= 0; --iBit)
    {
        m_DataOnHigh = !m_DataOnHigh;
        m_Gpio.Set(m_GpioPin, m_DataOnHigh);
        Sleep(Data & (0x1 << iBit) ? BIT1_LENGTH : BIT0_LENGTH);
    }
}

void C433MHz::EndSend()
{
    // flip the bit for a last time to make sure the receiver is able to detect the end of the last bit
    m_DataOnHigh = !m_DataOnHigh;
    m_Gpio.Set(m_GpioPin, m_DataOnHigh);
    Sleep(BIT0_LENGTH);

    // stop sending
    m_Gpio.SetLow(m_GpioPin);
}

void C433MHz::WaitForMagicByte()
{
    m_LastDataSwitch = m_SystemTimer.GetTicks();
    m_DataOnHigh = m_Gpio.IsHigh(m_GpioPin);
    uint8_t Byte = 0;
    while (Byte != MAGIC_BYTE)
    {
        bool PinIsHigh = m_Gpio.IsHigh(m_GpioPin);
        if (m_DataOnHigh != PinIsHigh)
        {
            // the data switched from low to high or high to low
            auto CurrTimestamp = m_SystemTimer.GetTicks();
            int DataTime = CurrTimestamp - m_LastDataSwitch;
            m_LastDataSwitch = CurrTimestamp;
            m_DataOnHigh = PinIsHigh;

            if (DataTime < BIT_THRESHOLD_LOW || BIT_THRESHOLD_HIGH < DataTime)
            {
                // we have invalid data which does not match our expected timing
                Byte = 0;
            }
            else
            {
                Byte = static_cast<uint8_t>((Byte << 1) & 0xFF);
                if (BIT_THRESHOLD_MID <= DataTime)
                {
                    Byte |= 0x1;
                }
            }
        }
    }
}

uint8_t C433MHz::ReceiveByte()
{
    uint8_t Byte = 0;
    int iBit = 0;
    while (iBit < 8)
    {
        bool PinIsHigh = m_Gpio.IsHigh(m_GpioPin);
        if (m_DataOnHigh != PinIsHigh)
        {
            // the data switched from low to high or high to low
            auto CurrTimestamp = m_SystemTimer.GetTicks();
            int DataTime = CurrTimestamp - m_LastDataSwitch;
            m_LastDataSwitch = CurrTimestamp;
            m_DataOnHigh = PinIsHigh;

            if (DataTime < BIT_THRESHOLD_LOW || BIT_THRESHOLD_HIGH < DataTime)
            {
                // we have invalid data which does not match our expected timing
                // this is probably some other sender blocking us
                throw std::runtime_error("Unexpected data timing.");
            }
            else
            {
                Byte = static_cast<uint8_t>((Byte << 1) & 0xFF);
                if (BIT_THRESHOLD_MID <= DataTime)
                {
                    Byte |= 0x1;
                }
                ++iBit;
            }
        }
    }
    return Byte;
}

void C433MHz::Sleep(unsigned int Microseconds)
{
    auto StartTimestamp = m_SystemTimer.GetTicks();
    while (m_SystemTimer.GetTicks() - StartTimestamp < Microseconds)
    {
        // wait until time elapsed
    }
}
