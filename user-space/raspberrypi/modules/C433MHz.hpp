#pragma once

#include <cstdint>

class CRaspberryPi;
class CGpio;
class CSystemTimer;

/*
 * This class provides an abstraction for a 433 MHz sender and receiver.
 * Do not use this class directly. Instead use C433MHzSender and C433MHzReceiver.
 */
class C433MHz
{
public:
    const uint8_t MAGIC_BYTE = 0b11001010; // some arbitrary pattern we are using to identify our own sender and receiver
    const int BIT0_LENGTH = 250; // length of a bit with value 0 in microseconds
    const int BIT1_LENGTH = BIT0_LENGTH * 2; // length of a bit with value 1 in microseconds
    const int BIT_THRESHOLD_LOW = BIT0_LENGTH / 2; // tolerances we accept for our bit timings
    const int BIT_THRESHOLD_MID = (BIT0_LENGTH + BIT1_LENGTH) / 2;
    const int BIT_THRESHOLD_HIGH = BIT1_LENGTH + BIT0_LENGTH;

public:
    /*
     * No need for copying objects of this class
     */
    C433MHz(const C433MHz&) = delete;
    C433MHz& operator=(const C433MHz&) = delete;

public:
    /*
     * Constructor
     * Provide an instance of the raspberry pi and the GPIO pin for the data connection.
     */
    C433MHz(CRaspberryPi& RaspberryPi, int GpioPin);

protected:
    /*
     * Helper method for sending arbitrary data. Provide a pointer to the data and the size in bytes as parameter.
     */
    void InternalSend(const void* Data, std::size_t Size);

    /*
     * Helper method for receiving arbitrary data. Provide a pointer to the data and the size in bytes as parameter.
     */
    void InternalReceive(void* Data, std::size_t Size);

private:
    /*
     * Calculates the data checksum. Provide a pointer to the data and the size in bytes as parameter. Returns the checksum.
     */
    uint8_t CalculateChecksum(const void* Data, std::size_t Size);

    /*
     * Signals the start of sending data
     */
    void StartSend();

    /*
     * Sends a single byte of data
     */
    void SendByte(uint8_t Data);

    /*
     * Signals the end of sending data
     */
    void EndSend();

    /*
     * Blocks until the magic byte is received
     */
    void WaitForMagicByte();

    /*
     * Receives a single byte of data
     */
    uint8_t ReceiveByte();

    /*
     * Blocks until the given time in microseconds has elapsed. Does a busy wait.
     */
    void Sleep(unsigned int Microseconds);

protected:
    CGpio& m_Gpio;
    CSystemTimer& m_SystemTimer;
    const int m_GpioPin;

private:
    bool m_DataOnHigh;
    uint32_t m_LastDataSwitch;

};
