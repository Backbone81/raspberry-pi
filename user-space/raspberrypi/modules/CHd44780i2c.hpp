#pragma once

#include <cstdint>
#include <string>

class CRaspberryPi;
class CGpio;
class CSystemTimer;
class CBsc;

/*
 * This class provides an abstraction for HD44780 displays with I2C adapter.
 *
 * The I2C bits map to the display pins:
 * Bit 0: R/S
 * Bit 1: R/W
 * Bit 2: E
 * Bit 3: Backlight enable
 * Bit 4: D4
 * Bit 5: D5
 * Bit 6: D6
 * Bit 7: D7
 */
class CHd44780i2c
{
public:
    /*
     * See datasheet chapter 7.1.1 page 5
     */
    const uint8_t MODULE_ADDRESS = 0x27;

public:
    /*
     * No need for copying objects of this class
     */
    CHd44780i2c(const CHd44780i2c&) = delete;
    CHd44780i2c& operator=(const CHd44780i2c&) = delete;

public:
    /*
     * Constructor
     * Provide an instance of the raspberry pi and the GPIO pins for the I2C connection.
     */
    CHd44780i2c(CRaspberryPi& RaspberryPi, int SdaGpioPin, int SclGpioPin);

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
     * Helper method for activating 4 bit bus transfer
     */
    void Enable4BitInterface();

    /*
     * Busy wait for 2 us. Needed for timing the communication protocol
     */
    void Wait();

    /*
     * Bring the display into the desired state. Used in the constructor only.
     */
    void Setup();

    /*
     * Helper function writing the lower 4 bit of the data to the display.
     */
    void Write4Bit(bool IsData, uint8_t Data);

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
    CBsc& m_Bsc;
    const int m_SdaGpioPin;
    const int m_SclGpioPin;

};
