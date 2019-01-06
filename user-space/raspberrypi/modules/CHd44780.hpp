#pragma once

#include <cstdint>
#include <string>

class CRaspberryPi;
class CGpio;
class CSystemTimer;

/*
 * This class provides an abstraction for HD44780 displays.
 */
class CHd44780
{
public:
    /*
     * No need for copying objects of this class
     */
    CHd44780(const CHd44780&) = delete;
    CHd44780& operator=(const CHd44780&) = delete;

public:
    /*
     * Constructor
     * Provide the GPIO pins as parameter where you connected the display pins.
     */
    CHd44780(CRaspberryPi& RaspberryPi, int RsGpioPin, int RwGpioPin, int EGpioPin, int Db0GpioPin, int Db1GpioPin, int Db2GpioPin, int Db3GpioPin, int Db4GpioPin, int Db5GpioPin, int Db6GpioPin, int Db7GpioPin);

    /*
     * Delete all content and get back to the first character in the first line.
     */
    void ClearDisplay();

    /*
     * Set the text for all four lines. Will cut texts which are too long.
     */
    void SetText(std::string Line1, std::string Line2 = "", std::string Line3 = "", std::string Line4 = "");

    /*
     * Return to the first character in the first line without changing the content.
     */
    void ReturnHome();

    /*
     * Set entry mode
     */
    void SetEntryMode(bool Increment, bool Shift);

    /*
     * Set display mode
     */
    void SetDisplay(bool On, bool Cursor, bool CursorBlink);

    /*
     * Set interface details
     */
    void SetFunction(bool Bus8Bit, bool DualLineDisplay, bool Font5x11);

    /*
     * Set DDRAM address for writing next
     */
    void SetDdramAddress(uint8_t Address);

private:
    /*
     * Busy wait for 2 us. Needed for timing the communication protocol
     */
    void Wait();

    /*
     * Bring the display into the desired state. Used in the constructor only.
     */
    void Setup();

    /*
     * Generic write helper which sends the given information to the display
     */
    void Write(bool IsData, uint8_t Data);

    /*
     * Writes the given instruction code to the display.
     */
    void WriteInstruction(uint8_t Data);

    /*
     * Writes the given data to the display.
     */
    void WriteData(uint8_t Data);

private:
    CGpio& m_Gpio;
    CSystemTimer& m_SystemTimer;
    const int m_RsGpioPin;
    const int m_RwGpioPin;
    const int m_EGpioPin;
    const int m_Db0GpioPin;
    const int m_Db1GpioPin;
    const int m_Db2GpioPin;
    const int m_Db3GpioPin;
    const int m_Db4GpioPin;
    const int m_Db5GpioPin;
    const int m_Db6GpioPin;
    const int m_Db7GpioPin;

};
