#include "CHd44780.hpp"

#include <chrono>
#include <thread>
#include "../CRaspberryPi.hpp"
#include "../CSystemTimer.hpp"

CHd44780::CHd44780(CRaspberryPi& RaspberryPi, int RsGpioPin, int RwGpioPin, int EGpioPin, int Db0GpioPin, int Db1GpioPin, int Db2GpioPin, int Db3GpioPin, int Db4GpioPin, int Db5GpioPin, int Db6GpioPin, int Db7GpioPin)
    : m_Gpio(RaspberryPi.Gpio()),
      m_SystemTimer(RaspberryPi.SystemTimer()),
      m_RsGpioPin(RsGpioPin),
      m_RwGpioPin(RwGpioPin),
      m_EGpioPin(EGpioPin),
      m_Db0GpioPin(Db0GpioPin),
      m_Db1GpioPin(Db1GpioPin),
      m_Db2GpioPin(Db2GpioPin),
      m_Db3GpioPin(Db3GpioPin),
      m_Db4GpioPin(Db4GpioPin),
      m_Db5GpioPin(Db5GpioPin),
      m_Db6GpioPin(Db6GpioPin),
      m_Db7GpioPin(Db7GpioPin)
{
    m_Gpio.SetOutput(m_RsGpioPin);
    m_Gpio.SetOutput(m_RwGpioPin);
    m_Gpio.SetOutput(m_EGpioPin);
    m_Gpio.SetOutput(m_Db0GpioPin);
    m_Gpio.SetOutput(m_Db1GpioPin);
    m_Gpio.SetOutput(m_Db2GpioPin);
    m_Gpio.SetOutput(m_Db3GpioPin);
    m_Gpio.SetOutput(m_Db4GpioPin);
    m_Gpio.SetOutput(m_Db5GpioPin);
    m_Gpio.SetOutput(m_Db6GpioPin);
    m_Gpio.SetOutput(m_Db7GpioPin);
    Setup();
}

void CHd44780::ClearDisplay()
{
    WriteInstruction(0b00000001);
}

void CHd44780::SetText(std::string Line1, std::string Line2, std::string Line3, std::string Line4)
{
    // we will cut strings which are too long for the display
    Line1 = Line1.substr(0, 20);
    Line2 = Line2.substr(0, 20);
    Line3 = Line3.substr(0, 20);
    Line4 = Line4.substr(0, 20);

    // clear display will return us to the first character in the first line
    ClearDisplay();
    for (auto iChar : Line1)
    {
        WriteData(iChar);
    }

    SetDdramAddress(0x40);
    for (auto iChar : Line2)
    {
        WriteData(iChar);
    }

    SetDdramAddress(0x14);
    for (auto iChar : Line3)
    {
        WriteData(iChar);
    }

    SetDdramAddress(0x54);
    for (auto iChar : Line4)
    {
        WriteData(iChar);
    }
}

void CHd44780::ReturnHome()
{
    WriteInstruction(0b00000010);
}

void CHd44780::SetEntryMode(bool Increment, bool Shift)
{
    uint8_t iIncrement = Increment ? 0b00000010 : 0;
    uint8_t iShift = Shift ? 0b00000001 : 0;
    WriteInstruction(0b00000100 | iIncrement | iShift);
}

void CHd44780::SetDisplay(bool On, bool Cursor, bool CursorBlink)
{
    uint8_t iDisplayOn = On ? 0b00000100 : 0;
    uint8_t iCursorOn = Cursor ? 0b00000010 : 0;
    uint8_t iCursorBlink = CursorBlink ? 0b00000001 : 0;
    WriteInstruction(0b00001000 | iDisplayOn | iCursorOn | iCursorBlink);
}

void CHd44780::SetFunction(bool Bus8Bit, bool DualLineDisplay, bool Font5x11)
{
    uint8_t iBus8Bit = Bus8Bit ? 0b00010000 : 0;
    uint8_t uDualLineDisplay = DualLineDisplay ? 0b00001000 : 0;
    uint8_t iFont5x11 = Font5x11 ? 0b00000100 : 0;
    WriteInstruction(0b00100000 | iBus8Bit | uDualLineDisplay | iFont5x11);
}

void CHd44780::SetDdramAddress(uint8_t Address)
{
    WriteInstruction(0b10000000 | (Address & 0b01111111));
}

void CHd44780::Wait()
{
    // we need nanosecond precision but we cannot measure so low
    // the wait times given in the datasheet are minimum times so there is no harm in waiting longer
    // to be safe we wait at least 2us, because a 1us could be less than 1 us when the clock tick
    // switches right now
    auto Start = m_SystemTimer.GetTicks();
    while (m_SystemTimer.GetTicks() - Start < 2)
    {
        // just loop until time is reached
    }
}

void CHd44780::Setup()
{
    SetFunction(true, false, false);
    SetFunction(true, true, false);
    SetDisplay(true, false, false);
}

void CHd44780::Write(bool IsData, uint8_t Data)
{
    m_Gpio.Set(m_RsGpioPin, IsData);
    m_Gpio.SetLow(m_RwGpioPin);
    Wait();

    m_Gpio.SetHigh(m_EGpioPin);
    m_Gpio.Set(m_Db0GpioPin, Data & 0b00000001);
    m_Gpio.Set(m_Db1GpioPin, Data & 0b00000010);
    m_Gpio.Set(m_Db2GpioPin, Data & 0b00000100);
    m_Gpio.Set(m_Db3GpioPin, Data & 0b00001000);
    m_Gpio.Set(m_Db4GpioPin, Data & 0b00010000);
    m_Gpio.Set(m_Db5GpioPin, Data & 0b00100000);
    m_Gpio.Set(m_Db6GpioPin, Data & 0b01000000);
    m_Gpio.Set(m_Db7GpioPin, Data & 0b10000000);
    Wait();

    m_Gpio.SetLow(m_EGpioPin);
    Wait();
}

void CHd44780::WriteInstruction(uint8_t Data)
{
    Write(false, Data);

    // after having sent the data, the display needs time for processing the instruction
    // the datasheet lists the longest execution time for instructions to be 2.16 ms which we round up to 3 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
}

void CHd44780::WriteData(uint8_t Data)
{
    Write(true, Data);

    // after having send the data the display need time for processing the instruction
    // the datasheet lists the longest execution time for data write instructions to be 53 us which we round up to 1 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
