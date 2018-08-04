#include <Arduino.h>
#include "Wheel.h"

Wheel::Wheel(SoftwareSerial9 *serial, uint32_t Baud, bool inv, int directionPin, int16_t speedLimit, double wheelDiameter)
:   speedLimit(speedLimit), 
    directionPin(directionPin), 
    Baudrate(Baud), 
    mySerial(serial), 
    wheelRadius(wheelDiameter/2)
{
    if(inv == true)
    {
        inverted = -1;
    }
    else
    {
        inverted = 1;
    }
    mySerial->begin(Baudrate);
}

void Wheel::SetSpeed(int16_t sp)
{
    currentSpeed = sp;
    SendSpeedOverUart(currentSpeed);
}

void Wheel::SetMps(double ms)
{
    SetRpm(ms/(Pi_30 * wheelRadius));
}

double Wheel::GetMps()
{
    return (GetRpm() * Pi_30 * wheelRadius);
}

void Wheel::Stop()
{
    SetSpeed(0);
}

double Wheel::GetRpm()
{
    return currentRpm;
}

void Wheel::RisingIsr()
{
    timeRisingTmp = micros();
}

void Wheel::FallingIsr()
{
    unsigned long currentTime = micros();
    if((currentTime - timeRisingTmp) > 200)
    {
        timeFallingTmp = currentTime;
        if(digitalRead(directionPin))
        {
            direction = Direction::FORWARD;
        }
        else
        {
            direction = Direction::BACKWARD;
        }
    }
    // Critical section
    noInterrupts();
    timeFalling = timeFallingTmp;
    timeRising = timeRisingTmp;
    pulsewidth = timeFalling - timeRising;
    currentRpm = CalculateRpm();
    interrupts();
}

double Wheel::CalculateRpm()
{
    double rpm;
    double p = (double)(timeFalling - timeRising);
    // Convert nonlinear pulsewidth to RPM
    /* used: http://polynomialregression.drque.net/online.php
    pulwewidth  RPM
    8340    	242.1
    8600	    230.9
    9060	    220.4
    9740	    209.2
    10300	    198.8
    10650	    187.8
    11370	    177.4
    12100	    166.1
    13000	    155.5
    14000	    144.4
    15100	    133.9
    16500	    122.7
    18280	    112.1
    20100	    100.9
    22360	    90.4
    25600	    79.3
    29700	    68.8
    45000	    46.5
    58890	    36
    79000	    26.7
    165000  	13.4 */
    rpm = 1546995.9100708 * pow(p,-0.97182638159083517718);

    if(direction == Direction::BACKWARD)
    {
        rpm *= -1.0;
    }

    return rpm;
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


void Wheel::SetRpm(double rpm)
{
    // Linear mapping from rpm to set value
    /* used: http://polynomialregression.drque.net/online.php
    RPM     Set Value
    242.1	600
    230.9	575
    220.4	550
    209.2	525
    198.8	500
    187.8	475
    177.4	450
    166.1	425
    155.5	400
    144.4	375
    133.9	350
    122.7	325
    112.1	300
    100.9	275
    90.4	250
    79.3	225
    68.8	200
    46.5	150
    36	    125
    26.7	100
    13.4	75   */
    int16_t newSpeed =  (int16_t)(2.306630778747887 * rpm + 41.718436623141145);
    SetSpeed(newSpeed);
}