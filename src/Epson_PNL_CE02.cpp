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

ControlPanel::ControlPanel(int oePin, int serOutPin, int clockPin, int serInPin, int shiftLoadPin)
{
    ControlPanel::serOutPin = serOutPin;
    ControlPanel::clockPin = clockPin;
    ControlPanel::shiftLoadPin = shiftLoadPin;

    pinMode(ControlPanel::shiftLoadPin, OUTPUT);
    pinMode(ControlPanel::clockPin, OUTPUT);
    pinMode(ControlPanel::serOutPin, INPUT);
}

byte ControlPanel::readButtons()
{
    int value;
    byte sequence = 0b00000000;

    digitalWrite(ControlPanel::shiftLoadPin, LOW);  // enables parallel inputs
    digitalWrite(ControlPanel::clockPin, LOW);      // start clock pin low
    digitalWrite(ControlPanel::clockPin, HIGH);     // set clock pin high, data loaded into SR
    digitalWrite(ControlPanel::shiftLoadPin, HIGH); // disable parallel inputs and enable serial output

    for (int i = 0; i < 8; i++)
    {
        value = !digitalRead(ControlPanel::serOutPin); // reads data from SR serial data out pin (invert cause output is HIGH)
        if (value)
        {
            int a = (1 << i);    // shifts bit to its proper place in sequence
            sequence = sequence | a; // combines data from shifted bits to form a single 8-bit number
        }
        digitalWrite(ControlPanel::clockPin, LOW);  // after each bit is logged
        digitalWrite(ControlPanel::clockPin, HIGH); // pulses clock to get next bit
    }

    return sequence;
}
