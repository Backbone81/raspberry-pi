/*
 * This example detects the state of a button connected to GPIO pin 4.
 */
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>

#include "CRaspberryPi.hpp"
#include "modules/CButton.hpp"

int main()
{
    try
    {
        CRaspberryPi Raspi;
        CButton Button(Raspi, 4);
        for (;;)
        {
            if (Button.IsDown())
            {
                std::cout << "Button is pressed" << std::endl;
            }
            else
            {
                std::cout << "Button is not pressed" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    catch (const std::exception& E)
    {
        std::cout << "ERROR: " << E.what() << std::endl;
        return 1;
    }
    return 0;
}
