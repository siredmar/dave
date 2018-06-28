
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
#include "Wheel.h"
#include "cmdparser.h"

int RightWheelInterruptPin = 2;
int RightWheelDirectionPin = 3;
int LeftWheelInterruptPin = 4;
int LeftWheelDirectionPin = 5;

SoftwareSerial9 RightWheelSerial(9, 11);
SoftwareSerial9 LeftWheelSerial(8, 10);
Wheel RightWheel(&RightWheelSerial, 31847, false, RightWheelDirectionPin);
Wheel LeftWheel(&LeftWheelSerial, 31847, false, LeftWheelDirectionPin);

std::string inString = "";
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
    attachInterrupt(digitalPinToInterrupt(RightWheelInterruptPin), LeftWheelFallingIsr, FALLING);
    LeftWheel.RisingIsr();
}
void LeftWheelFallingIsr()
{
    attachInterrupt(digitalPinToInterrupt(RightWheelInterruptPin), LeftWheelRisingIsr, RISING);
    LeftWheel.FallingIsr();
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
}

static signed short spWhl = 0;
static signed short spWhr = 0;
static int rpmWhl = 0;
static int rpmWhr = 0;
bool HandleCommand(wheelCommandType cmd)
{
    if(cmd.valid == true)
    {
        if(cmd.wheel == LEFT)
        {
            spWhl = cmd.speed;
            Serial.println("ACK");
        }
        else if(cmd.wheel == RIGHT)
        {
            spWhr = cmd.speed;
            Serial.println("ACK");
        }
        else if(cmd.wheel == BOTH)
        {
            spWhr = cmd.speed;
            spWhl = cmd.speed;
            Serial.println("ACK");
        }
        else if(cmd.wheel == NONE)
        {
            spWhr = 0;
            spWhl = 0;
            Serial.println("ACK");
        } 
        if(cmd.wheel == HELP)
        {
            Serial.println("Help:");
            Serial.println("Each command must be terminated by lf");
            Serial.println("If command has been understood ACK is sent");
            Serial.println("If command has not been understood NACK is sent");
            Serial.println("Available commands:");
            Serial.println("l,## -> Left wheel speed ## (+/-)");
            Serial.println("r,## -> Right wheel speed ## (+/-)");
            Serial.println("b,## -> Both wheels speed ## (+/-)");
            Serial.println("s -> Both wheels stop");
        }
        else
        {
            cmd.valid = false;
        }

    }
    return cmd.valid;
}

void loop()
{
    if(Serial.available())
    {
        int inChar = Serial.read();

        if (inChar != '\n')
        {
            inString += (char)inChar;
        }
        else
        {
            HandleCommand(parse(inString));
            inString = "";
        }
    }
    RightWheel.SetSpeed(spWhr);
    LeftWheel.SetSpeed(spWhl);
    rpmWhr = RightWheel.GetRpm();
    rpmWhl = LeftWheel.GetRpm();
    static long rightrpm = 0;
    if(RightWheel.GetRpm() != rightrpm)
    {
        rightrpm = RightWheel.GetRpm();
        Serial.println(rightrpm);
    }
    delayMicroseconds(delayUs);
}
