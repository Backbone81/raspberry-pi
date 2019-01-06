#pragma once

#include <cstdint>

#include "C433MHz.hpp"

/*
 * This class provides an abstraction for a 433 MHz receiver.
 */
class C433MHzReceiver : public C433MHz
{
public:
    /*
     * No need for copying objects of this class
     */
    C433MHzReceiver(const C433MHzReceiver&) = delete;
    C433MHzReceiver& operator=(const C433MHzReceiver&) = delete;

public:
    /*
     * Constructor
     * Provide an instance of the raspberry pi and the GPIO pin for the data connection.
     */
    C433MHzReceiver(CRaspberryPi& RaspberryPi, int GpioPin);

    /*
     * Receives data. Provide a pointer to the data and the size in bytes as parameter
     */
    void Receive(void* Data, std::size_t Size);

};
