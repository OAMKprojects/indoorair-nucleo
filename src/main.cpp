#include "mbed.h"
#include "DHT22.h"

int main()
{
    Sensor      temp_sensor(D8);
    DHT_info data;
    int error;

    wait_us(100000);

    while (true) 
    {
        error = temp_sensor.readValues(data);

        if (error == NO_ERROR || error == ERR_PARITY_BIT)
        {
            printf("Temperature : %i.%iC Humidity : %i.%i%%\n", 
                    data.temperature / 10, data.temperature % 10, 
                    data.humidity / 10, data.humidity % 10);
        }
        else {
            printf("Error nro: %i \n", error);
        }

        wait_us(1000000);
    }
}

