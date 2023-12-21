/*!
 * @file Epson_PNL_CE02.cpp
 *
 * Written by Xavier BRASSOUD.
 * MIT license
 */


#include <Arduino.h>
#include <stdint.h>
#include "Epson_PNL_CE02.h"

const char *keyName(Key key)
{
    const double bitPos = log(key) / log(2);
    return keyNames[(int)bitPos];
}

const bool isKeyPressed(uint8_t keys, Key key)
{
    return (keys & key) != 0; // Check if key pressed 0000{key}000, key>0 if set
}

ControlPanel::ControlPanel(int oePin, int serOutPin, int clockPin, int serInPin, int shiftLoadPin, unsigned long debounceDelay, unsigned long longPressDelay)
{
    ControlPanel::serOutPin = serOutPin;
    ControlPanel::clockPin = clockPin;
    ControlPanel::shiftLoadPin = shiftLoadPin;

    ControlPanel::debounceDelay = debounceDelay;
    ControlPanel::longPressDelay = longPressDelay;

    pinMode(ControlPanel::shiftLoadPin, OUTPUT);
    pinMode(ControlPanel::clockPin, OUTPUT);
    pinMode(ControlPanel::serOutPin, INPUT);
}


void ControlPanel::read()
{
    int value;
    byte dataIn = 0b00000000;

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
            dataIn = dataIn | a; // combines data from shifted bits to form a single 8-bit number
        }
        digitalWrite(ControlPanel::clockPin, LOW);  // after each bit is logged
        digitalWrite(ControlPanel::clockPin, HIGH); // pulses clock to get next bit
    }

    if (ControlPanel::pressedKeys != dataIn)
    {
        ControlPanel::pressedKeys = dataIn;
    }
    keysDispatcher(ControlPanel::pressedKeys);
}


// Dispatch keys on short/long pressed registers.
void ControlPanel::keysDispatcher(uint8_t keys)
{
    byte longPress = 0b00000000;
    byte shortPress = 0b00000000;

    for (int i = 0; i < 8; i++)
    {
        Key key = Key(0b00000000 | (1 << i));
        unsigned long *_keysTimers = keysTimers;
        bool wasPressed = _keysTimers[i] != 0;
        bool isPressed = isKeyPressed(keys, key);

        // Button pressed 1st time
        if (!wasPressed && isPressed)
        {
            _keysTimers[i] = millis();
        }
        // Button released, let's calculate his press delay in order to dispatch
        else if (wasPressed && !isPressed)
        {
            const unsigned long duration = millis() - _keysTimers[i];
            if (duration > debounceDelay)
            {
                if (duration < longPressDelay)
                {
                    Serial.print("Short press: ");
                    Serial.println(duration);

                    shortPress = shortPress | 1 << i;
                }
                else
                {
                    Serial.print("Long press: ");
                    Serial.println(duration);

                    longPress = longPress | (1 << i);
                }
            }
            _keysTimers[i] = 0;
        }
    }
    shortPressedKeys = shortPress;
    longPressedKeys = longPress;
}
