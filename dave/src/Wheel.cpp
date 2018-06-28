#include <Arduino.h>
#include "Wheel.h"

Wheel::Wheel(SoftwareSerial9 *serial, uint32_t Baud, bool inv, int directionPin)
{
    mySerial = serial;
    Baudrate = Baud;
    if(inv == true)
    {
        inverted = -1;
    }
    else
    {
        inverted = 1;
    }
    mySerial->begin(Baudrate);
    this->directionPin = directionPin;
    currentTime = 0;
}

Wheel::~Wheel(){ }

void Wheel::SetSpeed(int16_t sp)
{
    currentSpeed = sp;
    SendSpeedOverUart(currentSpeed);
}

void Wheel::Stop()
{
    SetSpeed(0);
}

void Wheel::IncreaseSpeed(int delta)
{
    currentSpeed += delta;
    SetSpeed(currentSpeed);
}

void Wheel::DecreaseSpeed(int delta)
{
    currentSpeed -= delta;
    SetSpeed(currentSpeed);
}

void Wheel::SendSpeedOverUart(int16_t sp)
{
    if(sp > 600)
        sp = 600;
    sp *= inverted;
    mySerial->write9(256);
    mySerial->write9(sp & 0xFF);
    mySerial->write9((sp >> 8) & 0xFF);
    mySerial->write9(sp & 0xFF);
    mySerial->write9((sp >> 8) & 0xFF);
    mySerial->write9(85);
    mySerial->write9(82);
    mySerial->write9(82);
}

long Wheel::GetRpm()
{
    return currentRpm;
}

void Wheel::RisingIsr()
{
    timeOld = micros();
}

void Wheel::FallingIsr()
{
    if((micros() - timeOld) > 100)
    {
        pulsewidth = micros() - timeOld;
        if(digitalRead(directionPin))
        {
            direction = Direction::FORWARD;
        }
        else
        {
            direction = Direction::BACKWARD;
        }
        CalculateRpm();
    }
    
}

long Wheel::CalculateRpm()
{
    // http://www.xuru.org/rt/PR.asp#Manually
    /*
        Time  SetValue
        10000 500 
        13000 400 
        14900 350 
        18000 300 
        22000 250 
        29500 200 
        35000 175 
        44000 150 
        58000 125 
        77500 100 
        16400 75 
    */
    // y = 1.607826538·10-25 x6 - 4.242648736·10-20 x5 + 4.526767364·10-15 x4 - 2.526320447·10-10 x3 + 7.942832572·10-6 x2 - 1.400295492·10-1 x + 1317.549516
    float p = (double)pulsewidth;
    currentRpm = 1.607826538e-25 * (p*p*p*p*p*p) 
    - 4.242648736e-20 * (p*p*p*p*p)
    + 4.526767364e-15 * (p*p*p*p)
    - 2.526320447e-10 * (p*p*p)
    + 7.942832572e-6 * (p*p)
    - 1.400295492e-1 * p
    + 1317.549516;
    if(direction == Direction::BACKWARD)
    {
        currentRpm *= -1;
    }
    return currentRpm;
}