/* Copyright (C) 2017 Armin Schlegel. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").
*/

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial9.h>
#include "Wheel.h"
#include "cmdparser.h"

SoftwareSerial9 LeftWheelSerial(8, 10);
SoftwareSerial9 RightWheelSerial(9, 11);
Wheel LeftWheel(&LeftWheelSerial, 31847, false);
Wheel RightWheel(&RightWheelSerial, 31847, false);

String inString = "";
unsigned int delayUs = 200;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
}

bool HandleCommand(wheelCommandType cmd)
{
    if(cmd.valid == true)
    {
        if(cmd.wheel == LEFT)
        {
            std::cout << "Left: " << cmd.speed << std::endl;
        }
        else if(cmd.wheel == RIGHT)
        {
            std::cout << "Right: " << cmd.speed << std::endl;
        }
        else if(cmd.wheel == BOTH)
        {
            std::cout << "Both: " << cmd.speed << std::endl;
        }
    }
    return cmd.valid;
}

void loop()
{
    static signed short spWhl = 0;
    static signed short spWhr = 0;
    if(Serial.available())
    {
        int inChar = Serial.read();

        if (inChar != '\n')
        {
            inString += (char)inChar;
        }
        else
        {
            // TODO handle input with cmdparser
            inString = "";
            received_byte = 0;
        }

    }

    delayMicroseconds(delayUs);
    RightWheel.SetSpeed(spWhl);
    LeftWheel.SetSpeed(spWhr);
}
