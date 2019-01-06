#include "CBsc.hpp"

#include <stdexcept>

CBsc::CBsc(void* Base)
    : m_Register(reinterpret_cast<uint32_t*>(Base)),
      m_Control(m_Register),
      m_Status(m_Register + 1),
      m_DataLength(m_Register + 2),
      m_SlaveAddress(m_Register + 3),
      m_DataFifo(m_Register + 4)
{

}

void CBsc::Read(uint8_t ModuleAddress, uint8_t RegisterAddress, void* Data, std::size_t Size)
{
    *m_SlaveAddress = ModuleAddress;

    // tell the module what we want to read
    // the order of clear status, set data length and start transfer is important
    // if you start with write transfer, you get spurious failings of the I2C communication
    ClearStatus();
    SetDataLength(sizeof(RegisterAddress));
    StartWriteTransfer();
    WriteFifo(&RegisterAddress, sizeof(RegisterAddress));
    WaitUntilDone();

    // read the requested data
    // the order of clear status, set data length and start transfer is important
    // if you start with write transfer, you get spurious failings of the I2C communication
    ClearStatus();
    SetDataLength(Size);
    StartReadTransfer();
    ReadFifo(Data, Size);
    WaitUntilDone();
}

void CBsc::Write(uint8_t ModuleAddress, uint8_t RegisterAddress, const void* Data, std::size_t Size)
{
    *m_SlaveAddress = ModuleAddress;

    // the order of clear status, set data length and start transfer is important
    // if you start with write transfer, you get spurious failings of the I2C communication
    ClearStatus();
    SetDataLength(Size + sizeof(RegisterAddress));
    StartWriteTransfer();
    WriteFifo(&RegisterAddress, sizeof(RegisterAddress));
    WriteFifo(Data, Size);
    WaitUntilDone();
}

void CBsc::ClearStatus()
{
    // set the bits for CLKT, ERR and DONE to clear all old notifications
    *m_Status = (0x1 << 9) | (0x1 << 8) | (0x1 << 1);
}

void CBsc::SetDataLength(std::size_t Length)
{
    if (0xFFFF < Length)
    {
        throw std::runtime_error("Maximum data length exceeded.");
    }
    *m_DataLength = Length;
}

void CBsc::WriteFifo(const void* Data, std::size_t Size)
{
    const uint8_t* TypedData = static_cast<const uint8_t*>(Data);
    for (std::size_t iByte = 0; iByte < Size; ++iByte, ++TypedData)
    {
        WaitForWritePossible();
        *m_DataFifo = *TypedData;
    }
}

void CBsc::ReadFifo(void* Data, std::size_t Size)
{
    uint8_t* TypedData = static_cast<uint8_t*>(Data);
    for (std::size_t iByte = 0; iByte < Size; ++iByte, ++TypedData)
    {
        WaitForReadPossible();
        *TypedData = *m_DataFifo & 0xFF;
    }
}

void CBsc::StartWriteTransfer()
{
    // set the I2CEN, ST, CLEAR and READ bits
    *m_Control = (0x1 << 15) | (0x1 << 7) | (0x1 << 4);
}

void CBsc::StartReadTransfer()
{
    // set the I2CEN, ST, CLEAR and READ bits
    *m_Control = (0x1 << 15) | (0x1 << 7) | (0x1 << 4) | 0x1;
}

bool CBsc::IsReadPossible() const
{
    // check RXD bit
    return *m_Status & (0x1 << 5);
}

void CBsc::WaitForReadPossible() const
{
    clock_t Start = clock();
    while (!IsReadPossible())
    {
        if (CLOCKS_PER_SEC < clock() - Start)
        {
            throw std::runtime_error("Timeout while waiting for read to be possible.");
        }
    }
}

bool CBsc::IsWritePossible() const
{
    // check TXD bit
    return *m_Status & (0x1 << 4);
}

void CBsc::WaitForWritePossible() const
{
    clock_t Start = clock();
    while (!IsWritePossible())
    {
        if (CLOCKS_PER_SEC < clock() - Start)
        {
            throw std::runtime_error("Timeout while waiting for write to be possible.");
        }
    }
}

bool CBsc::IsDone() const
{
    return *m_Status & (0x1 << 1);
}

void CBsc::WaitUntilDone() const
{
    clock_t Start = clock();
    while (!IsDone())
    {
        if (CLOCKS_PER_SEC < clock() - Start)
        {
            throw std::runtime_error("Waiting for end of transfer timed out.");
        }
    }
}
