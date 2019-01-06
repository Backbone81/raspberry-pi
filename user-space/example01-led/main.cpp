/*
 * This example blinks the LED connected to GPIO pin 4 five times.
 */
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>

#include "CRaspberryPi.hpp"
#include "modules/CLed.hpp"

int main()
{
    try
    {
        CRaspberryPi Raspi;
        CLed Led(Raspi, 4);
        for (int i = 0; i < 5; ++i)
        {
            std::cout << "LED on" << std::endl;
            Led.On();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "LED off" << std::endl;
            Led.Off();
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
