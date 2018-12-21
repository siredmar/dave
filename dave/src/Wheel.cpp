#include "Wheel.h"
#include <Arduino.h>
#include <PID_v1.h>

Wheel::Wheel(SoftwareSerial9* serial, uint32_t Baud, int directionPin, const WheelConfig& config)
    : mySerial(serial)
    , Baudrate(Baud)
    , directionPin(directionPin)
    , config(config)
{
    mySerial->begin(Baudrate);
    myPID = new PID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);
    myPID->SetOutputLimits(-600, 600);
    myPID->SetMode(AUTOMATIC);
    Setpoint = 0.0;
    pulseWidth = 2000000.0;
    Input = 0.0;
}

void Wheel::SetNewSetpoint(double newSet)
{
    static double oldSet = -1000.0;
    if (newSet != oldSet)
    {
        oldSet = newSet;
        Setpoint = newSet;
        // Serial.print("New set value [RPM]: ");
        // Serial.println(newSet);
    }
}

double Wheel::Calculate()
{
    // static uint16_t loop = 0;
    // if (loop++ % 10000 == 0)
    // {
    //     loop = 0;
    //     Serial.print("pulseWidth: ");
    //     Serial.println(pulseWidth);
    //     Serial.print("Input: ");
    //     Serial.println(Input);
    //     Serial.print("Output: ");
    //     Serial.println(Output);
    // }
    // Close the loop: Calculate RPM from averaged pulse width and feed it into the Input of the PID

    Input = CalculateRpm();
    myPID->Compute();
    // Serial.println((int16_t)Output);
    SendSpeedOverUart((int16_t)Output);
}

double Wheel::CalculateRpm()
{
    double rpm;
    // Convert nonlinear pulsewidth to RPM
    // used: http://polynomialregression.drque.net/online.php
    if (direction == Direction::BACKWARD)
    {
        rpm = config.backward.PulseWidthToRpmCoeff * pow(pulseWidth, config.backward.PulseWidthToRpmFactor);
        rpm *= -1.0;
    }
    else
    {
        rpm = config.forward.PulseWidthToRpmCoeff * pow(pulseWidth, config.forward.PulseWidthToRpmFactor);
    }
    return rpm;
}

double Wheel::GetPulseWidth()
{
    return pulseWidth;
}

void Wheel::RisingIsr()
{
    if (risingIsrAllowed == true)
    {
        timeRisingTmp = micros();
        risingIsrAllowed = false;
    }
}

void Wheel::FallingIsr()
{
    unsigned long currentTime = micros();
    double p = (double)(currentTime - timeRisingTmp);
    if (p < 5000)
    {
        Serial.println(p);
        return;
    }

    // timeFallingTmp = currentTime;
    if (digitalRead(directionPin))
    {
        direction = Direction::FORWARD;
    }
    else
    {
        direction = Direction::BACKWARD;
    }

    // Moving Average for the measured pulse width to get rid of unwanted spikes and make the PID more rigid
    noInterrupts();
    pulseWidth = MovingAveragePulseWidth(p);
    risingIsrAllowed = true;
    interrupts();
}

double Wheel::GetRpm()
{
    return currentRpm;
}

// ---------------------------------------------------- //
// -------------------- Privates ---------------------- //
// ---------------------------------------------------- //

double Wheel::MovingAveragePulseWidth(double value)
{
    return filter.add(value);
}

void Wheel::SendSpeedOverUart(int16_t sp)
{
    if (sp > config.speedLimit)
    {
        sp = config.speedLimit;
    }

    if (config.WheelDirection == Direction::BACKWARD)
    {
        sp *= -1;
    }
    mySerial->write9(256);
    mySerial->write9(sp & 0xFF);
    mySerial->write9((sp >> 8) & 0xFF);
    mySerial->write9(sp & 0xFF);
    mySerial->write9((sp >> 8) & 0xFF);
    mySerial->write9(85);
    mySerial->write9(82);
    mySerial->write9(82);
}
