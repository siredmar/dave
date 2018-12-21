
// /* Copyright (C) 2017 Armin Schlegel. All rights reserved.

// This software may be distributed and modified under the terms of the GNU
// General Public License version 2 (GPL2) as published by the Free Software
// Foundation and appearing in the file GPL2.TXT included in the packaging of
// this file. Please note that GPL2 Section 2[b] requires that all works based
// on this software must also be made publicly available under the terms of
// the GPL2 ("Copyleft").
// */

#include "Wheel.h"
#include <Arduino.h>
#include <Cmd.h>
#include <SoftwareSerial9.h>
#include <Wire.h>

int RightWheelInterruptPin = 2;
int LeftWheelInterruptPin = 3;
int RightWheelDirectionPin = 4;
int LeftWheelDirectionPin = 5;

SoftwareSerial9 RightWheelSerial(9, 11);
SoftwareSerial9 LeftWheelSerial(8, 10);

Wheel::DirectionConfig RightForwardConfig(2.4712743625212092, 43.83397427549102, 1536870.4806951, -0.97083491860268806164);
Wheel::DirectionConfig RightBackwardConfig(2.47886548902593, 42.7854284956262, 2374763.2755623, -1.01702098812953985391);
Wheel::DirectionConfig LeftForwardConfig(2.4645062876692854, 40.69502685546491, 2048351.0840305, -1.00317552451016697449);
Wheel::DirectionConfig LeftBackwardConfig(2.4803416290933966, 39.34038762270925, 1810713.9946028, -0.98944796937319023658);

Wheel::WheelConfig RightWheelConfig(RightForwardConfig, RightBackwardConfig, Wheel::Direction::FORWARD, 600, 0.17);
Wheel::WheelConfig LeftWheelConfig(LeftForwardConfig, LeftBackwardConfig, Wheel::Direction::FORWARD, 600, 0.17);

Wheel RightWheel(&RightWheelSerial, 31847, RightWheelDirectionPin, RightWheelConfig);
Wheel LeftWheel(&LeftWheelSerial, 31847, LeftWheelDirectionPin, LeftWheelConfig);

unsigned int delayUs = 500;

void RightWheelRisingIsr();
void RightWheelFallingIsr();
void LeftWheelRisingIsr();
void LeftWheelFallingIsr();

void RightWheelRisingIsr()
{
    attachInterrupt(digitalPinToInterrupt(RightWheelInterruptPin), RightWheelFallingIsr, FALLING);
    RightWheel.RisingIsr();
}

void RightWheelFallingIsr()
{
    attachInterrupt(digitalPinToInterrupt(RightWheelInterruptPin), RightWheelRisingIsr, RISING);
    RightWheel.FallingIsr();
}

void LeftWheelRisingIsr()
{
    attachInterrupt(digitalPinToInterrupt(LeftWheelInterruptPin), LeftWheelFallingIsr, FALLING);
    LeftWheel.RisingIsr();
}
void LeftWheelFallingIsr()
{
    attachInterrupt(digitalPinToInterrupt(LeftWheelInterruptPin), LeftWheelRisingIsr, RISING);
    LeftWheel.FallingIsr();
}

static double mpsl = 0.0;
static double mpsr = 0.0;

void SetRight(double val)
{
    mpsr = val;
    Serial.print("Right RPM: ");
    Serial.print(mpsr);
}

void SetLeft(double val)
{
    mpsl = val;
    Serial.print("Left RPM: ");
    Serial.print(mpsr);
}

void LeftWheelCb(int arg_cnt, char** args)
{
    double mps;

    if (arg_cnt > 1)
    {
        mps = cmdStr2double(args[1]);
        SetLeft(mps);
        mpsl = mps;
    }
    else
    {
        SetLeft(0.0);
    }
}

void RightWheelCb(int arg_cnt, char** args)
{
    double mps;

    if (arg_cnt > 1)
    {
        mps = cmdStr2double(args[1]);
        SetRight(mps);
    }
    else
    {
        SetRight(0.0);
    }
}

void BothWheelsCb(int arg_cnt, char** args)
{
    double mps;

    if (arg_cnt > 1)
    {
        mps = cmdStr2double(args[1]);
        SetRight(mps);
        SetLeft(mps);
        Serial.println(mps);
    }
    else
    {
        SetRight(0.0);
        SetLeft(0.0);
    }
}

void StopCb(int arg_cnt, char** args)
{
    mpsr = 0.0;
    mpsl = 0.0;
}

void GetCb(int arg_cnt, char** args)
{
    double rightms = RightWheel.GetRpm();
    Serial.print("/Wheel/Right/Speed/");
    Serial.println(rightms);

    double leftms = LeftWheel.GetRpm();
    Serial.print("/Wheel/Left/Speed/");
    Serial.println(leftms);
}

void InfoCb(int arg_cnt, char** args)
{
    Serial.print("Right pulseWidth: ");
    Serial.println(RightWheel.GetPulseWidth());
    Serial.print("Left pulseWidth: ");
    Serial.println(LeftWheel.GetPulseWidth());
}

void HelpCb(int arg_cnt, char** args)
{
    Serial.println("Available commands");
    Serial.println("r #\t\tright wheel speed [m/s]");
    Serial.println("l #\t\tleft wheel speed [m/s]");
    Serial.println("b #\t\tboth wheels speed [m/s]");
    Serial.println("s\t\tstop both wheels");
    Serial.println("getmotors\tgets current motor speeds");
    Serial.println("h\t\tget this help");
}

void setup()
{
    pinMode(RightWheelInterruptPin, INPUT);
    pinMode(RightWheelDirectionPin, INPUT);
    pinMode(LeftWheelInterruptPin, INPUT);
    pinMode(LeftWheelDirectionPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(RightWheelInterruptPin), RightWheelRisingIsr, RISING);
    attachInterrupt(digitalPinToInterrupt(LeftWheelInterruptPin), LeftWheelRisingIsr, RISING);
    Serial.begin(115200);
    Wire.begin();
    cmdInit(&Serial);
    cmdAdd("l", LeftWheelCb);
    cmdAdd("r", RightWheelCb);
    cmdAdd("b", BothWheelsCb);
    cmdAdd("s", StopCb);
    cmdAdd("h", HelpCb);
    // cmdAdd("getmotors", GetCb);
    cmdAdd("g", GetCb);
    cmdAdd("i", InfoCb);
}

void loop()
{
    cmdPoll();
    // RightWheel.SendSpeedOverUart((int16_t)mpsr);
    // LeftWheel.SendSpeedOverUart((int16_t)mpsl);
    RightWheel.SetNewSetpoint(mpsr);
    RightWheel.Calculate();
    LeftWheel.SetNewSetpoint(mpsl);
    LeftWheel.Calculate();
    delayMicroseconds(delayUs);
}
