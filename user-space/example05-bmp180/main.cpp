/*
 * This example uses a BMP180 sensor for measureing temperature and pressure.
 */
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>

#include "CRaspberryPi.hpp"
#include "modules/CBmp180.hpp"

int main()
{
    try
    {
        CRaspberryPi Raspi;
        CBmp180 Bmp180(Raspi, 2, 3);
        for (int i = 0; i < 10; ++i)
        {
            auto Data = Bmp180.Measure();
            std::cout << "Temperature: " << Data.Temperature << " °C" << std::endl;
            std::cout << "Pressure: " << Data.Pressure / 100.0f << " hPa" << std::endl;
            std::cout << std::endl;
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
