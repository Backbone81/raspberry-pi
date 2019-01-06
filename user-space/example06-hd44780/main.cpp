/*
 * This example uses a HD44780 display for displaying text.
 */
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>

#include "CRaspberryPi.hpp"
#include "modules/CHd44780.hpp"

int main()
{
    try
    {
        CRaspberryPi Raspi;
        CHd44780 Display(Raspi, 21, 20, 26, 16, 19, 13, 6, 12, 5, 25, 24);
        Display.SetText("Temp: 21.5C", "Humid: 68%", "Prsssure: 1018 mbar", "Cloudy");
    }
    catch (const std::exception& E)
    {
        std::cout << "ERROR: " << E.what() << std::endl;
        return 1;
    }
    return 0;
}
