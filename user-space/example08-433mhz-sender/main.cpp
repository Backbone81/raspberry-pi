/*
 * This example uses a 433 MHz sender for sending data.
 */
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "CRaspberryPi.hpp"
#include "modules/C433MHzSender.hpp"

int main()
{
    try
    {
        CRaspberryPi Raspi;
        C433MHzSender Sender(Raspi, 4);
        for (int i = 0; i < 10000; ++i)
        {
            int SomeRandomNumber = rand();
            std::cout << "Sending: " << SomeRandomNumber << std::endl;
            Sender.Send(&SomeRandomNumber, sizeof(SomeRandomNumber));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception& E)
    {
        std::cout << "ERROR: " << E.what() << std::endl;
        return 1;
    }
    return 0;
}
