#pragma once

/*
 * The data structure returning the results of a measurement
 */
struct dht11_data
{
    int temperature; // temperature in celcius
    int humidity; // humidity in percent
};

/*
 * Initialize DHT11 sensor. Provide the GPIO pin we use for communication as parameter.
 */
int dht11_init(int pin);

/*
 * Cleanup everything. We are done with the DHT11
 */
void dht11_shutdown(void);

/*
 * Does a measurement and returns the measured data.
 */
struct dht11_data dht11_measure(void);
