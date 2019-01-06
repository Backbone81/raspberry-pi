#include "dht11.h"

#include <linux/delay.h> // needed for mdelay
#include <linux/mutex.h> // needed for DEFINE_MUTEX, mutex_lock, mutex_unlock

#include "system_timer.h"
#include "gpio.h"
#include "logger.h"

// some helper macro for processing error conditions in dht11_measure
#define EXIT_ON_ERROR(call) if (call == -1) { goto out; }

 // the GPIO pin we are using for communication with the DHT11 sensor
static int dht11_pin = 0;

// the system timer timestamp of the last measurement. this is used
// to respect the minimum timespan of 2 seconds between measurements
// and the minimum 1 second after power up. otherwise the sensor does
// not return any data.
static u32 last_data_reading = 0;

// this mutex is used to prevent multiple processes from triggering
// a measurement at the same time. this would lead to currupted data
// being transferred
static DEFINE_MUTEX(dht11_mutex);

void dht11_init(int pin)
{
    DEBUG_OUT("DHT11 init");
    dht11_pin = pin;
    gpio_set_output(dht11_pin);
    gpio_set_high(dht11_pin);

    // DHT11 sensor needs 1 second after power up
    // by setting last_data_reading to the current timestamp the next data
    // reading will not happen within the next 2 seconds
    last_data_reading = system_timer_ticks();
}

void dht11_shutdown(void)
{
    DEBUG_OUT("DHT11 shutdown");
    gpio_set_low(dht11_pin);
}

static u32 dht11_wait_for_low(void)
{
    u32 start_time = system_timer_ticks();
    while (gpio_is_high(dht11_pin))
    {
        if (system_timer_frequency() < system_timer_ticks() - start_time)
        {
            ERROR_OUT("Timeout while waiting for pin to get low");
            return -1;
        }
    }
    return system_timer_ticks() - start_time;
}

static u32 dht11_wait_for_high(void)
{
    u32 start_time = system_timer_ticks();
    while (gpio_is_low(dht11_pin))
    {
        if (system_timer_frequency() < system_timer_ticks() - start_time)
        {
            ERROR_OUT("Timeout while waiting for pin to get high");
            return -1;
        }
    }
    return system_timer_ticks() - start_time;
}

static u8 dht11_read_byte(void)
{
    u8 data = 0;
    u32 low_time, high_time;
    int i = 0;
    for (i = 0; i < 8; ++i)
    {
        low_time = dht11_wait_for_high();
        if (low_time == -1)
        {
            return -1;
        }

        high_time = dht11_wait_for_low();
        if (high_time == -1)
        {
            return -1;
        }

        data <<= 1;
        if (low_time < high_time)
        {
            data |= 0x1;
        }
    }
    return data;
}

static void dht11_send_start_signal(void)
{
    u32 current_timestamp = system_timer_ticks();
    if (current_timestamp - last_data_reading < system_timer_frequency() * 2)
    {
        // wait until we have at least 2 seconds since the last measurement
        // otherwise the sensor will not respond
        u32 usecs_to_wait = system_timer_frequency() * 2 - (current_timestamp - last_data_reading);
        msleep(usecs_to_wait / 1000 + 1);
    }
    gpio_set_low(dht11_pin);
    mdelay(19); // msleep is not precise enough and will sleep much longer which causes issues with DHT11 sending nothing
    gpio_set_high(dht11_pin);
}

static u8 calculate_checksum(u8 humindity_high, u8 humidity_low, u8 temperature_high, u8 temperature_low)
{
    return humindity_high + humidity_low + temperature_high + temperature_low;
}

struct dht11_data dht11_measure(void)
{
    u8 humidity_high, humdity_low, temperature_high, temperature_low, checksum;
    struct dht11_data result = {
        .temperature = 0,
        .humidity = 0
    };
    mutex_lock(&dht11_mutex); // prevent other processes from starting a measurement while we are at it
    DEBUG_OUT("DHT11 measurement");

    dht11_send_start_signal();
    gpio_set_input(dht11_pin);
    EXIT_ON_ERROR(dht11_wait_for_low());
    EXIT_ON_ERROR(dht11_wait_for_high());
    EXIT_ON_ERROR(dht11_wait_for_low());

    humidity_high = dht11_read_byte();
    EXIT_ON_ERROR(humidity_high);
    humdity_low = dht11_read_byte();
    EXIT_ON_ERROR(humdity_low);
    temperature_high = dht11_read_byte();
    EXIT_ON_ERROR(temperature_high);
    temperature_low = dht11_read_byte();
    EXIT_ON_ERROR(temperature_low);
    checksum = dht11_read_byte();
    EXIT_ON_ERROR(checksum);

    EXIT_ON_ERROR(dht11_wait_for_high());
    gpio_set_output(dht11_pin);
    gpio_set_high(dht11_pin);

    if (checksum != calculate_checksum(humidity_high, humdity_low, temperature_high, temperature_low))
    {
        ERROR_OUT("Parity checksum failed");
        goto out;
    }
    result.temperature = temperature_high;
    result.humidity = humidity_high;

out:
    gpio_set_output(dht11_pin);
    gpio_set_high(dht11_pin);
    last_data_reading = system_timer_ticks();
    mutex_unlock(&dht11_mutex);
    return result;
}
