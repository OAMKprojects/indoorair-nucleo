#include "mbed.h"
#include "DHT22.h"
#include <cstdio>

#define ADMIN_APP       // define this if admin app is used
#define NTC_SENSOR      // define this if NTC-sensor is available

#ifdef ADMIN_APP

#define COM_ADMIN_CONNECT    'A'
#define COM_ADMIN_DISCONNECT 'D'
#define COM_RESTART          'R'

bool admin_connected = false;
char buffer[256];
BufferedSerial serial(USBTX, USBRX);
Timer uptimer;

#ifdef NTC_SENSOR

#define RESISTOR_OHM  10000
#define READ_TIMES    10
#define C1            1.340933450906873e-3
#define C2            2.020648784092252e-4
#define C3            1.958312651e-7

float R1 = RESISTOR_OHM;
float R2;

float getCelcius(float raw)
{
    R2 = raw / (1.0 - raw) * R1;
    double logR = log(R2);
    double temp = 1.0 / (C1 + C2 * logR + C3 * pow(logR, 3)) - 273.15;
    return temp;
}

#endif // NTC_SENSOR
#endif // ADMIN_APP

int main()
{
    Sensor temp_sensor(D8);
    DHT_info data;
    int error;

    #ifdef ADMIN_APP
    uptimer.start();

    #ifdef NTC_SENSOR
    AnalogIn dht(A0);
    float ntc_temp;
    float raw_value;
    #endif // NTC_SENSOR
    #endif // ADMIN_APP

    wait_us(100000);

    while (true) 
    {
        error = temp_sensor.readValues(data);

        #ifdef ADMIN_APP

        buffer[0] = '\0';
        while (serial.readable()) {
            int len = serial.read(&buffer, 255);
            buffer[len] = '\0';

            for (auto &ch : buffer) {
                switch (ch) {
                    case COM_ADMIN_CONNECT:
                        admin_connected = true;
                        break;
                    case COM_ADMIN_DISCONNECT:
                        admin_connected = false;
                        break;
                    case COM_RESTART:
                        NVIC_SystemReset();
                        break;
                }
            }
        }
        #ifdef NTC_SENSOR
        raw_value = 0;
        for (int i = 0; i < READ_TIMES; i++) {
            raw_value += dht.read();
        }
        raw_value /= READ_TIMES;
        ntc_temp = getCelcius(raw_value);

        if (error == NO_ERROR || error == ERR_PARITY_BIT) {
            printf("{\"uptime\":\"%llu\",\"temperature\":%i.%i,\"humidity\":%i.%i,\"NTC temperature\":%i.%i,\"NTC resistance\":%i}\n",
                    uptimer.elapsed_time().count() / 1000000,
                    data.temperature / 10, data.temperature % 10, 
                    data.humidity / 10, data.humidity % 10,
                    (int)ntc_temp, 
                    abs((int)(ntc_temp * 10) % 10),
                    (int)R2);
        }
        else {
        printf("{\"uptime\":\"%llu\",\"NTC temperature\":%i.%i,\"NTC resistance\":%i,\"error\":\"%s\"}\n",
                    uptimer.elapsed_time().count() / 1000000,
                    (int)ntc_temp, 
                    abs((int)(ntc_temp * 10) % 10),
                    (int)R2,
                    temp_sensor.getErrorString());
        }
        #else
        if (error == NO_ERROR || error == ERR_PARITY_BIT)
        {
            
            printf("{\"uptime\":\"%llu\",\"temperature\":%i.%i,\"humidity\":%i.%i}\n",
                    uptimer.elapsed_time().count() / 1000000,
                    data.temperature / 10, data.temperature % 10, 
                    data.humidity / 10, data.humidity % 10);
        }
        else {
            printf("{\"error\":\"%s\"}\n", temp_sensor.getErrorString());
        }
        #endif // NTC_SENSOR
        #else

        if (error == NO_ERROR || error == ERR_PARITY_BIT)
        {
            printf("{\"temperature\": %i.%i, \"humidity\": %i.%i}\n", 
                    data.temperature / 10, abs(data.temperature % 10), 
                    data.humidity / 10, data.humidity % 10);
        }
        else {
            printf("{\"error\":\"%s\"}\n", temp_sensor.getErrorString());
        }
        #endif // ADMIN_APP
        wait_us(1000000);
    }
}

