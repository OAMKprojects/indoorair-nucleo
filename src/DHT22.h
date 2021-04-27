#ifndef DHT22_h
#define DHT22_h

#include "mbed.h"
#include <cstdint>
#include <string>

#define T_WAKE_UP           200
#define T_WAIT_SENSOR       100
#define T_READ_SENSOR       10000

#define M_WAKE_UP           0
#define M_WAIT_SENSOR       1
#define M_READY_TO_READ     2
#define M_READ_LOW          3
#define M_READ_HIGH         4

#define NO_ERROR            0
#define ERR_NO_SENSOR       1
#define ERR_TIMEOUT_1       2
#define ERR_TIMEOUT_2       3
#define ERR_PARITY_BIT      4

struct DHT_info
{
    int16_t temperature;
    int16_t humidity; 
};

class Sensor
{
    public:
        Sensor(PinName sda_pin);
        ~Sensor();

        int         readValues(DHT_info &info);
        int         readPin() {return dht_sensor->read();} 
        const char *getErrorString();

    private:
        bool wakeSensor();
        bool storeBit();
        void checkParity();
        
        DigitalInOut *dht_sensor;
        Timer        timer;

        uint8_t      err_no;
        uint8_t      bit_count;
        uint8_t      temp_lo, temp_hi;
        uint8_t      hum_lo, hum_hi;
        uint8_t      parity_bit;
        uint64_t     bits;
        int          bit_time;
};

Sensor::Sensor(PinName sda_pin)
{
    err_no = 0;
    dht_sensor = new DigitalInOut(sda_pin);
    dht_sensor->mode(PullNone);
}

Sensor::~Sensor()
{
    delete(dht_sensor);
}

const char *Sensor::getErrorString()
{
    const char *error_string[] {
        "ok",
        "no sensor",
        "timeout 1",
        "timeout 2",
        "parity bit"
    };

    return error_string[err_no];
}

bool Sensor::wakeSensor()
{
    timer.reset();
    dht_sensor->output();
    dht_sensor->write(0);
    wait_us(1000);
    dht_sensor->write(1);
    dht_sensor->input();

    int timeout = 0;
    int mode = M_WAKE_UP;

    while(1)
    {
        wait_us(1);
        ++timeout;

        switch(mode) 
        {
            case M_WAKE_UP:
                if (timeout > T_WAKE_UP) {err_no = ERR_NO_SENSOR; return false;}
                if (!dht_sensor->read()) {timeout = 0; mode = M_WAIT_SENSOR;}
                break;

            case M_WAIT_SENSOR:
                if (timeout > T_WAIT_SENSOR) {err_no = ERR_TIMEOUT_1; return false;}
                if (dht_sensor->read()) return true;
                break;
        }
    }
}

bool Sensor::storeBit()
{
    bits <<= 1;
    if (bit_time > 30) bits |= 1;
    if (++bit_count == 40) return true;

    return false;
}

void Sensor::checkParity()
{
    parity_bit = bits & 0xFF;
    bits >>= 8;
    temp_lo = bits & 0xFF;
    bits >>= 8;
    temp_hi = bits & 0xFF;
    bits >>= 8;
    hum_lo = bits & 0xFF;
    bits >>= 8;
    hum_hi = bits;

    if ((temp_lo + temp_hi + hum_lo + hum_hi) != parity_bit) err_no = ERR_PARITY_BIT;
}

int Sensor::readValues(DHT_info &info)
{
    err_no = 0;

    if (!wakeSensor()) return err_no;

    bool end_loop = false;
    int timeout = 0;
    int mode = M_READY_TO_READ;
    bit_count = 0;
    bits = 0;

    while(!end_loop)
    {
        wait_ns(100);
        if (++timeout == T_READ_SENSOR)
        {
            err_no = ERR_TIMEOUT_2;
            return false;
        }

        switch(mode) 
        {
            case M_READY_TO_READ:
                if (!dht_sensor->read()) mode = M_READ_LOW;
                break;

            case M_READ_LOW:
                if (dht_sensor->read()) {
                    mode = M_READ_HIGH;
                    bit_time = 0;
                }
                break;

            case M_READ_HIGH:
                ++bit_time;

                if (!dht_sensor->read()) {
                    end_loop = storeBit();
                    mode = M_READ_LOW;
                }
                break;
        }
    }

    checkParity();

    info.humidity = (hum_hi << 8) | hum_lo;
    info.temperature = (temp_hi << 8) | temp_lo;

    return err_no;
}

#endif