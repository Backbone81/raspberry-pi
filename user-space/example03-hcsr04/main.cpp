/*
 * This example uses a HC-SR04 ultrasonic distance measurement module to detect the distance to some object.
 */
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>

#include "CRaspberryPi.hpp"
#include "modules/CHcsr04.hpp"

int main()
{
    try
    {
        CRaspberryPi Raspi;
        CHcsr04 Sensor(Raspi, 17, 12);
        for (int i = 0; i < 10; ++i)
        {
            std::cout << "Distance is " << Sensor.GetDistance() << " meters" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    catch (const std::exception& E)
    {
        std::cout << "ERROR: " << E.what() << std::endl;
        return 1;
    }
    return 0;
}
