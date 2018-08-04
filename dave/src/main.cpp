
// /* Copyright (C) 2017 Armin Schlegel. All rights reserved.

// This software may be distributed and modified under the terms of the GNU
// General Public License version 2 (GPL2) as published by the Free Software
// Foundation and appearing in the file GPL2.TXT included in the packaging of
// this file. Please note that GPL2 Section 2[b] requires that all works based
// on this software must also be made publicly available under the terms of
// the GPL2 ("Copyleft").
// */

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial9.h>
#include <Cmd.h>
#include "Wheel.h"

int RightWheelInterruptPin = 2;
int LeftWheelInterruptPin = 3;
int RightWheelDirectionPin = 4;
int LeftWheelDirectionPin = 5;

SoftwareSerial9 RightWheelSerial(9, 11);
SoftwareSerial9 LeftWheelSerial(8, 10);
Wheel RightWheel(&RightWheelSerial, 31847, false, RightWheelDirectionPin, 600, 0.17);
Wheel LeftWheel(&LeftWheelSerial, 31847, false, LeftWheelDirectionPin, 600, 0.17);

unsigned int delayUs = 200;

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

void LeftWheelCb(int arg_cnt, char **args)
{
    double mps;
  
    if (arg_cnt > 1)
    {  
        mps = cmdStr2double(args[1]);
        mpsl = mps;
    }
    else
    {
        mpsl = 0.0;
    }
}

void RightWheelCb(int arg_cnt, char **args)
{
    double mps;
  
    if (arg_cnt > 1)
    {  
        mps = cmdStr2double(args[1]);
        mpsr = mps;
    }
    else
    {
        mpsr = 0.0;
    }
}

void BothWheelsCb(int arg_cnt, char **args)
{
    double mps;
  
    if (arg_cnt > 1)
    {  
        mps = cmdStr2double(args[1]);
        mpsl = mps;
        mpsr = mps;
        Serial.println(mps);
    }
    else
    {
        mpsr = 0.0;
        mpsl = 0.0;
    }
}

void StopCb(int arg_cnt, char **args)
{
    mpsr = 0.0;
    mpsl = 0.0;
}

void GetCb(int arg_cnt, char **args)
{
    double rightms = RightWheel.GetMps();
    Serial.print("/Wheel/Right/Speed/");
    Serial.println(rightms);

    double leftms = LeftWheel.GetMps();
    Serial.print("/Wheel/Left/Speed/");
    Serial.println(leftms);
}

void HelpCb(int arg_cnt, char **args)
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
    cmdAdd("getmotors", GetCb);
}

void loop()
{
    cmdPoll();
    RightWheel.SetMps(mpsr);
    LeftWheel.SetMps(mpsl);
    delayMicroseconds(delayUs);
}
