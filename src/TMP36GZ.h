#ifndef TMP36GZ_h
#define TMP36GZ_h

#include "mbed.h"

#define SOUND_SPEED     0.340   // (mm / us)

class TempSensor
{
    public:
        TempSensor(PinName in);
        ~TempSensor();

        uint16_t read();

    private:
        AnalogIn  *temp_pin;
};

TempSensor::TempSensor(PinName in)
{
    temp_pin = new AnalogIn(in);
}

TempSensor::~TempSensor()
{
    delete temp_pin;
}

uint16_t TempSensor::read()
{
    return temp_pin->read_u16();
}

#endif