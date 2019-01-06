/*
 * This example uses a HD44780 display with i2c adapter for displaying text.
 */
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>

#include "CRaspberryPi.hpp"
#include "modules/CHd44780i2c.hpp"

int main()
{
    try
    {
        CRaspberryPi Raspi;
        CHd44780i2c Display(Raspi, 2, 3);
        Display.SetText("Temp: 21.5C", "Humid: 68%", "Pressure: 1018 mbar", "Cloudy");
    }
    catch (const std::exception& E)
    {
        std::cout << "ERROR: " << E.what() << std::endl;
        return 1;
    }
    return 0;
}
