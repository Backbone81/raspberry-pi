#include "CHd44780i2c.hpp"

#include <chrono>
#include <thread>
#include "../CRaspberryPi.hpp"
#include "../CSystemTimer.hpp"

CHd44780i2c::CHd44780i2c(CRaspberryPi& RaspberryPi, int SdaGpioPin, int SclGpioPin)
    : m_Gpio(RaspberryPi.Gpio()),
      m_SystemTimer(RaspberryPi.SystemTimer()),
      m_Bsc(RaspberryPi.Bsc1()),
      m_SdaGpioPin(SdaGpioPin),
      m_SclGpioPin(SclGpioPin)
{
    m_Gpio.SetAlternateFunction(0, m_SdaGpioPin);
    m_Gpio.SetAlternateFunction(0, m_SclGpioPin);
    Setup();
}

void CHd44780i2c::ClearDisplay()
{
    WriteInstruction(0b00000001);
}

void CHd44780i2c::SetText(std::string Line1, std::string Line2, std::string Line3, std::string Line4)
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

void CHd44780i2c::ReturnHome()
{
    WriteInstruction(0b00000010);
}

void CHd44780i2c::SetEntryMode(bool Increment, bool Shift)
{
    uint8_t iIncrement = Increment ? 0b00000010 : 0;
    uint8_t iShift = Shift ? 0b00000001 : 0;
    WriteInstruction(0b00000100 | iIncrement | iShift);
}

void CHd44780i2c::SetDisplay(bool On, bool Cursor, bool CursorBlink)
{
    uint8_t iDisplayOn = On ? 0b00000100 : 0;
    uint8_t iCursorOn = Cursor ? 0b00000010 : 0;
    uint8_t iCursorBlink = CursorBlink ? 0b00000001 : 0;
    WriteInstruction(0b00001000 | iDisplayOn | iCursorOn | iCursorBlink);
}

void CHd44780i2c::SetFunction(bool Bus8Bit, bool DualLineDisplay, bool Font5x11)
{
    uint8_t iBus8Bit = Bus8Bit ? 0b00010000 : 0;
    uint8_t uDualLineDisplay = DualLineDisplay ? 0b00001000 : 0;
    uint8_t iFont5x11 = Font5x11 ? 0b00000100 : 0;
    WriteInstruction(0b00100000 | iBus8Bit | uDualLineDisplay | iFont5x11);
}

void CHd44780i2c::SetDdramAddress(uint8_t Address)
{
    WriteInstruction(0b10000000 | (Address & 0b01111111));
}

void CHd44780i2c::Enable4BitInterface()
{
    Write4Bit(false, 0b0010);
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
}

void CHd44780i2c::Wait()
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

void CHd44780i2c::Setup()
{
    Enable4BitInterface();
    SetFunction(false, true, false);
    SetDisplay(true, false, false);
}

void CHd44780i2c::Write4Bit(bool IsData, uint8_t Data)
{
    Data = static_cast<uint8_t>(Data << 4); // we need our data in the high bits

    uint8_t Rs = IsData ? 0b00000001 : 0;
    uint8_t Rw = 0;
    uint8_t Backlight = 0b00001000;
    // The module does not have any registers we can write to. Instead every byte we send it
    // will directly toggle the corresponding pins. Because of this, we transfer our data
    // thorugh the register parameter and we set the data parameters to null.
    m_Bsc.Write(MODULE_ADDRESS, Rs | Rw | Backlight, nullptr, 0);
    Wait();

    uint8_t E = 0b00000100;
    m_Bsc.Write(MODULE_ADDRESS, Rs | Rw | E | Backlight | Data, nullptr, 0);
    Wait();

    m_Bsc.Write(MODULE_ADDRESS, Rs | Rw | Backlight | Data, nullptr, 0);
    Wait();
}

void CHd44780i2c::Write(bool IsData, uint8_t Data)
{
    uint8_t UpperBits = static_cast<uint8_t>(Data >> 4);
    uint8_t LowerBits = (Data & 0b00001111);
    Write4Bit(IsData, UpperBits);
    Write4Bit(IsData, LowerBits);
}

void CHd44780i2c::WriteInstruction(uint8_t Data)
{
    Write(false, Data);

    // after having sent the data, the display needs time for processing the instruction
    // the datasheet lists the longest execution time for instructions to be 2.16 ms which we round up to 3 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
}

void CHd44780i2c::WriteData(uint8_t Data)
{
    Write(true, Data);

    // after having send the data the display need time for processing the instruction
    // the datasheet lists the longest execution time for data write instructions to be 53 us which we round up to 1 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
