/**
 * @file Epson_PNL_CE02.h
 * @author Xavier BRASSOUD (contact@xavierbrassoud.fr)
 * @brief Library to repurposing the control panel (PNL CE02) of EPSON XP 520/530/540
 * printers from an Arduino.
 *
 * These control board use:
 *  => LV165 shift register for buttons (pins 2, 9, 11)
 *  => 8-bit parallel for display (pins 1, 6, 9, 10, 11, 13) through 2 shifts registers:
 *      * VHC595: clock + commands
 *      * VHC164: 8 bits of parallel data (D0 - D7)
 *
 * The FFC from the CPU to the control board has this pinout:
 * | Pin | Purpose                                   |
 * |-----|-------------------------------------------|
 * | 1   | 3-STATE Output Enable Input (OE)          |
 * | 2   | Serial Data Output (SER OUT)              |
 * | 3   | GND                                       |
 * | 4   | Power button                              |
 * | 5   | 3.3V supply                               |
 * | 6   | LCD reset                                 |
 * | 7   | LCD backlight (+5V !)                     |
 * | 8   | GND                                       |
 * | 9   | Shift Register Clock Input (SCK)          |
 * | 10  | Serial Data Input (SER IN)                |
 * | 11  | Storage Register Clock Input (RCK)        |
 * | 12  | GND                                       |
 * | 13  | LCD write                                 |
 * | 14  | GND                                       |
 *
 *
 * @version 0.0
 * @date 2023-12-22
 *
 * @copyright MIT license
 */

#include <Arduino.h>
#include <stdint.h>
#include "Epson_PNL_CE02.h"

// least significant bit order
enum BitPosition {
    POWER_LED = 1,
};

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

Epson_PNL_CE02::Epson_PNL_CE02(int oePin, int serOutPin, int powerButtonPin, int lcdResetPin, int clockPin, int serInPin, int latchPin, int lcdWritePin)
{
    Epson_PNL_CE02::oePin = oePin;
    Epson_PNL_CE02::serOutPin = serOutPin;
    Epson_PNL_CE02::powerButtonPin = powerButtonPin;
    Epson_PNL_CE02::lcdResetPin = lcdResetPin;
    Epson_PNL_CE02::clockPin = clockPin;
    Epson_PNL_CE02::serInPin = serInPin;
    Epson_PNL_CE02::latchPin = latchPin;
    Epson_PNL_CE02::lcdWritePin = lcdWritePin;

    pinMode(Epson_PNL_CE02::powerButtonPin, INPUT);

    pinMode(Epson_PNL_CE02::oePin, OUTPUT);
    pinMode(Epson_PNL_CE02::latchPin, OUTPUT);
    pinMode(Epson_PNL_CE02::clockPin, OUTPUT);
    pinMode(Epson_PNL_CE02::serInPin, OUTPUT);
    pinMode(Epson_PNL_CE02::serOutPin, INPUT);
}

void Epson_PNL_CE02::synchronize()
{
    digitalWrite(latchPin, LOW); // enables parallel inputs

    // send buffer
    shiftOut(serInPin, clockPin, LSBFIRST, buffer);

    digitalWrite(clockPin, LOW);  // start clock pin low
    digitalWrite(clockPin, HIGH); // set clock pin high, data loaded into SR
    digitalWrite(latchPin, HIGH); // disable parallel inputs and enable serial output

    // read buttons
    buttonsSequence = 0b11111111 ^ shiftIn(serOutPin, clockPin, LSBFIRST); // (invert cause output is HIGH)
}

bool Epson_PNL_CE02::isPowerButtonPressed()
{
    return digitalRead(powerButtonPin) == HIGH;
}

uint8_t Epson_PNL_CE02::readPowerLed()
{
    return bitRead(buffer, POWER_LED);
}

void Epson_PNL_CE02::writePowerLed(uint8_t state)
{
    bitWrite(buffer, POWER_LED, state);
}
