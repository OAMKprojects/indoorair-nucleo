#ifndef HCSR04_h
#define HCSR04_h

#include "mbed.h"

#define SOUND_SPEED     0.340   // (mm / us)

class Ultrasonic
{
    public:
        Ultrasonic(PinName out, PinName in);
        ~Ultrasonic();

        int distance();

    private:
        DigitalOut  *burst_pin;
        DigitalIn   *echo_pin;
        Timer        timer;
};

Ultrasonic::Ultrasonic(PinName out, PinName in)
{
    burst_pin = new DigitalOut(out);
    echo_pin = new DigitalIn(in);

    echo_pin->mode(PullNone);
}

Ultrasonic::~Ultrasonic()
{
    delete burst_pin;
    delete echo_pin;
}

int Ultrasonic::distance()
{
    timer.reset();

    burst_pin->write(1);
    wait_us(10);
    burst_pin->write(0);

    int timeout = 100000;

    while(--timeout && !echo_pin->read());
    timer.start();

    if (!timeout) 
    {
        timer.stop(); 
        return -1;
    }
    
    timeout = 100000;

    while(--timeout && echo_pin->read());
    timer.stop();

    if (!timeout) return -1;

    int distance = (float)timer.elapsed_time().count() * SOUND_SPEED / 2.0;

    return distance;
}

#endif