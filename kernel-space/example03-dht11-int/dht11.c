#include <linux/interrupt.h> // needed for request_irq, free_irq
#include <linux/gpio/consumer.h> // needed for GPIO
#include <linux/timer.h> // needed for setup_timer, mod_timer, del_timer
#include <linux/delay.h> // needed for mdelay

#include "system_timer.h"
#include "dht11.h"
#include "logger.h"

// the interval time in seconds in which the driver reads new data from the DHT11 sensor
// if this is less than 2 the sensor will not provide any data
#define UPDATE_PERIOD 5

// when debugging the interrupts it is difficult to put log messages into them because they
// screw the timing of the interrupts. instead we use an GPIO output pin to follow what
// the interrupt is detecting. that way we can check with an oscilloscope how the interrupts
// are following the input pin events
// uncomment the next line to enable the debug pin on the GPIO pin stated there
// #define ENABLE_DEBUG_PIN 27

#ifdef ENABLE_DEBUG_PIN
// debug GPIO pin handle
static struct gpio_desc* debug_pin = NULL;
#endif

// data GPIO pin handle
static struct gpio_desc* data_pin = NULL;

// data GPIO pin interrupt number
static int data_pin_irq = 0;

// the timestamp the data pin last transitioned from high to low
// accessed only by the interrupt handler
static u32 last_data_down = 0;

// the timestamp the data pin last transitioned from low to high
// accessed only by the interrupt handler
static u32 last_data_up = 0; 

// detected bits are shifted into this buffer as they are detected
// accessed only by the interrupt handler
static u64 data_buffer = 0; 

// this counter is used to keep track of when the last bit of data arrived
// accessed by the interrupt handler and when we trigger a new measurement
static atomic_t bit_counter = ATOMIC_INIT(0);

// the last measurement data is shared between multiple threads/interrupts and need therefore be
// secured against race conditions.
static DEFINE_SPINLOCK(last_measurement_lock);

// this is the last measurement result, access must always be protected through last_measurement_lock
// accessed by the interrupt handler and when reading the measurement
static struct dht11_data last_measurement = {
    .temperature = 0,
    .humidity = 0
};

// this timer is used to trigger periodic update of the measurement data
static struct timer_list start_signal_timer;

// helper structure which holds the data we received from the sensor
struct split_data
{
    u8 humidity_high;
    u8 humidity_low;
    u8 temperature_high;
    u8 temperature_low;
    u8 checksum;
};

static struct split_data dht11_split_data(u64 data)
{
    struct split_data result = {
        .humidity_high = (data >> 32) & 0xFF,
        .humidity_low = (data >> 24) & 0xFF,
        .temperature_high = (data >> 16) & 0xFF,
        .temperature_low = (data >> 8) & 0xFF,
        .checksum = data & 0xFF
    };
    return result;
}

static u8 dht11_calculate_checksum(u8 humindity_high, u8 humidity_low, u8 temperature_high, u8 temperature_low)
{
    return humindity_high + humidity_low + temperature_high + temperature_low;
}

static void dht11_update_measurement(u64 data)
{
    unsigned long flags;
    struct split_data my_split_data = dht11_split_data(data);
    if (my_split_data.checksum != dht11_calculate_checksum(my_split_data.humidity_high, my_split_data.humidity_low, my_split_data.temperature_high, my_split_data.temperature_low))
    {
        ERROR_OUT("Data checksum failed. Not updating the measured data.");
        return;
    }

    // update the central data
    spin_lock_irqsave(&last_measurement_lock, flags);
    last_measurement.humidity = my_split_data.humidity_high;
    last_measurement.temperature = my_split_data.temperature_high;
    spin_unlock_irqrestore(&last_measurement_lock, flags);

    INFO_OUT("Temperature %i C / Humidity %i %%", my_split_data.temperature_high, my_split_data.humidity_high);
}

static irqreturn_t dht11_irq(int irq, void *dev_id)
{
    int curr_value = 0;
    u32 curr_timestamp = 0;
    if (dev_id != &last_measurement)
    {
        // not our interrupt handler
        return IRQ_NONE;
    }

    // get the current value of the input pin
    curr_value = gpiod_get_value(data_pin);
    if (curr_value < 0)
    {
        ERROR_OUT("Unable to retrieve current value of data GPIO pin");
        return IRQ_HANDLED;
    }

#ifdef ENABLE_DEBUG_PIN
    // make our debug pin follow the input pin
    gpiod_set_value(debug_pin, curr_value);
#endif

    curr_timestamp = system_timer_ticks();
    if (curr_value == 0)
    {
        // data pin is down, we have the end of a full bit
        data_buffer <<= 1;
        if (last_data_up - last_data_down < curr_timestamp - last_data_up)
        {
            data_buffer |= 0x1;
        }
        last_data_down = curr_timestamp;

        if (atomic_inc_return(&bit_counter) == 43)
        {
            // last data bit was received, process data
            // there is the first fake bit when we start to signal the start secquence (pin low)
            // at the end of our start sequence we have the second bit
            // then we have the response signal as third bit
            // afterwards come the 40 data bits which contain temperature, humidity and checksum
            // this makes 43 bits in total we wait to see
            dht11_update_measurement(data_buffer);
        }
    }
    else
    {
        // data pin is up
        last_data_up = curr_timestamp;
    }
    return IRQ_HANDLED;
}

static void dht11_start_signal_callback(unsigned long data)
{
    int error = 0;
    DEBUG_OUT("Retrieving data");

    // schedule our next update in UPDATE_PERIOD seconds
    mod_timer(&start_signal_timer, jiffies + msecs_to_jiffies(UPDATE_PERIOD * 1000));

    atomic_set(&bit_counter, 0);
    error = gpiod_direction_output(data_pin, 0);
    if (error)
    {
        ERROR_OUT("Unable to set data pin to output.");
    }

    // TO DO: This does a busy wait for 19 milliseconds which is a long time in kernel space.
    //        We can not use any sleep here because we are in an atomic context.
    //        Scheduling a different timer will be significantly off our time we want to wait.
    mdelay(19);

    error = gpiod_direction_input(data_pin);
    if (error)
    {
        ERROR_OUT("Unable to set data pin to input.");
        gpiod_direction_output(data_pin, 1);
        return;
    }
}

int dht11_init(int pin)
{
    int error;
    DEBUG_OUT("DHT11 init");

    // prepare data GPIO pin
    data_pin = gpio_to_desc(pin);
    if (data_pin == NULL)
    {
        ERROR_OUT("Unable to get data pin GPIO descriptor");
        goto fail;
    }
    error = gpiod_direction_output(data_pin, 1);
    if (error)
    {
        ERROR_OUT("Unable to set data pin GPIO direction");
        goto fail;
    }

#ifdef ENABLE_DEBUG_PIN
    // prepare debug GPIO pin
    debug_pin = gpio_to_desc(ENABLE_DEBUG_PIN);
    if (debug_pin == NULL)
    {
        ERROR_OUT("Unable to get debug pin GPIO descriptor");
        goto fail;
    }
    error = gpiod_direction_output(debug_pin, 1);
    if (error)
    {
        ERROR_OUT("Unable to set debug pin GPIO direction");
        goto fail;
    }
#endif

    // install interrupt
    data_pin_irq = gpiod_to_irq(data_pin);
    if (data_pin_irq < 0)
    {
        ERROR_OUT("Unable to retrieve IRQ number for data pin");
        goto fail;
    }
    error = request_irq(data_pin_irq, dht11_irq, IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "example03-dht11-int", &last_measurement);
    if (error)
    {
        ERROR_OUT("Unable to request interrupt");
        goto fail;
    }

    // start our first measurement 1 second after startup
    // if we start earlier the sensor will not return any data
    setup_timer(&start_signal_timer, dht11_start_signal_callback, 0);
    mod_timer(&start_signal_timer, jiffies + msecs_to_jiffies(1000));
    return 0;

fail:
#ifdef ENABLE_DEBUG_PIN
    if (debug_pin != NULL)
    {
        gpiod_put(debug_pin);
    }
#endif
    if (data_pin != NULL)
    {
        gpiod_put(data_pin);
    }
    return -1;
}

void dht11_shutdown(void)
{
    DEBUG_OUT("DHT11 shutdown");
    del_timer(&start_signal_timer);
    free_irq(data_pin_irq, &last_measurement);
#ifdef ENABLE_DEBUG_PIN
    gpiod_put(debug_pin);
#endif
    gpiod_put(data_pin);
}

struct dht11_data dht11_measure(void)
{
    unsigned long flags;
    struct dht11_data measurement_data;

    spin_lock_irqsave(&last_measurement_lock, flags);
    measurement_data = last_measurement;
    spin_unlock_irqrestore(&last_measurement_lock, flags);

    return measurement_data;
}
