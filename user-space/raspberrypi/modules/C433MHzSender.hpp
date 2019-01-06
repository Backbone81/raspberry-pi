#pragma once

#include <cstdint>

#include "C433MHz.hpp"

/*
 * This class provides an abstraction for a 433 MHz sender.
 */
class C433MHzSender : public C433MHz
{
public:
    /*
     * No need for copying objects of this class
     */
    C433MHzSender(const C433MHzSender&) = delete;
    C433MHzSender& operator=(const C433MHzSender&) = delete;

public:
    /*
     * Constructor
     * Provide an instance of the raspberry pi and the GPIO pin for the data connection.
     */
    C433MHzSender(CRaspberryPi& RaspberryPi, int GpioPin);

    /*
     * Sends data. Provide a pointer to the data and the size in bytes as parameter.
     */
    void Send(const void* Data, std::size_t Size);

};
