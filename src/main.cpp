#include "mbed.h"

#define RESISTOR_OHM  10000
#define READ_TIMES    10

#define C1            1.340933450906873e-3
#define C2            2.020648784092252e-4
#define C3            1.958312651e-7

float R1 = RESISTOR_OHM;
float R2;
float raw;

float getCelcius(float raw_value)
{
    R2 = raw / (1.0 - raw) * R1;
    double logR = log(R2);
    double temp = 1.0 / (C1 + C2 * logR + C3 * pow(logR, 3)) - 273.15;
    return temp;
}

int main()
{
    AnalogIn dht(A0);
    int i;
    double temp;

    while (true) 
    {
        raw = 0;
        for (i = 0; i < READ_TIMES; i++) {
            raw += dht.read();
            wait_us(1000);
        }
        
        raw /= READ_TIMES;
        temp = getCelcius(raw);

        printf("Resistance: %i ohms, Temperature: %i.%i C\n", (int)R2, (int)temp, (int)(temp * 10) % 10);

        ThisThread::sleep_for(1s);
    }
}
