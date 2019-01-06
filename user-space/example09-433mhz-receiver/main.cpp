/*
 * This example uses a 433 MHz sender for sending data.
 */
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "CRaspberryPi.hpp"
#include "modules/C433MHzReceiver.hpp"

int main()
{
    try
    {
        CRaspberryPi Raspi;
        C433MHzReceiver Receiver(Raspi, 4);
        for (int i = 0; i < 100; ++i)
        {
            int SomeNumber = 0;
            Receiver.Receive(&SomeNumber, sizeof(SomeNumber));
            std::cout << "Received: " << SomeNumber << std::endl;
        }
    }
    catch (const std::exception& E)
    {
        std::cout << "ERROR: " << E.what() << std::endl;
        return 1;
    }
    return 0;
}
