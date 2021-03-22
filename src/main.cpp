#include "mbed.h"
#include "DHT22.h"
#include "HCSR04.h"
#include "TMP36GZ.h"

int main()
{
    Sensor      temp_sensor(D8);
    Ultrasonic  dist_sensor(D11, D15);
    TempSensor  analog_temp(A0);

    DHT_info data;
    int error;
    int distance;

    wait_us(100000);

    while (true) 
    {
        error = temp_sensor.readValues(data);
        distance = dist_sensor.distance();

        if (error == NO_ERROR || error == ERR_PARITY_BIT)
        {
            printf("Temperature : %i.%iC Humidity : %i.%i%% Distance: %i.%i cm Analog Temperature: %i V\n", data.temperature / 10, data.temperature % 10, 
                                                                                data.humidity / 10, data.humidity % 10, 
                                                                                distance / 10, distance % 10,
                                                                                analog_temp.read());
        }
        else {
            printf("Error nro: %i \n", error);
        }

        wait_us(1000000);
    }
}

