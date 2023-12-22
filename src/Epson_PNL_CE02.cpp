/*!
 * @file Epson_PNL_CE02.cpp
 *
 * Written by Xavier BRASSOUD.
 * MIT license
 */


#include <Arduino.h>
#include <stdint.h>
#include "Epson_PNL_CE02.h"

const char *buttonNames[8] = {
    "Right",
    "Ok",
    "Up",
    "Left",
    "Start",
    "Down",
    "Stop",
    "Home",
};

const char *buttonName(ButtonFlag flag)
{
    const double bitPos = log(flag) / log(2);
    return buttonNames[(int)bitPos];
}

const bool isButtonPressed(uint8_t sequence, ButtonFlag flag)
{
    return (sequence & flag) != 0; // Check if key pressed 0000{key}000, key>0 if set
}

Epson_PNL_CE02::Epson_PNL_CE02(int oePin, int serOutPin, int clockPin, int serInPin, int shiftLoadPin)
{
    Epson_PNL_CE02::serOutPin = serOutPin;
    Epson_PNL_CE02::clockPin = clockPin;
    Epson_PNL_CE02::shiftLoadPin = shiftLoadPin;

    pinMode(Epson_PNL_CE02::shiftLoadPin, OUTPUT);
    pinMode(Epson_PNL_CE02::clockPin, OUTPUT);
    pinMode(Epson_PNL_CE02::serOutPin, INPUT);
}

byte Epson_PNL_CE02::readButtons()
{
    int value;
    byte sequence = 0b00000000;

    digitalWrite(Epson_PNL_CE02::shiftLoadPin, LOW);  // enables parallel inputs
    digitalWrite(Epson_PNL_CE02::clockPin, LOW);      // start clock pin low
    digitalWrite(Epson_PNL_CE02::clockPin, HIGH);     // set clock pin high, data loaded into SR
    digitalWrite(Epson_PNL_CE02::shiftLoadPin, HIGH); // disable parallel inputs and enable serial output

    for (int i = 0; i < 8; i++)
    {
        value = !digitalRead(Epson_PNL_CE02::serOutPin); // reads data from SR serial data out pin (invert cause output is HIGH)
        if (value)
        {
            int a = (1 << i);    // shifts bit to its proper place in sequence
            sequence = sequence | a; // combines data from shifted bits to form a single 8-bit number
        }
        digitalWrite(Epson_PNL_CE02::clockPin, LOW);  // after each bit is logged
        digitalWrite(Epson_PNL_CE02::clockPin, HIGH); // pulses clock to get next bit
    }

    return sequence;
}
