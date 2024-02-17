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
 * | Pin | Purpose                                   | MEGA 2560     |
 * |-----|-------------------------------------------|---------------|
 * | 1   | 3-STATE Output Enable Input (OE)          | 45            |
 * | 2   | Serial Data Output (SER OUT)              | 50 (SPI MISO) |
 * | 3   | GND                                       | GND           |
 * | 4   | Power button                              | 46 🔺         |
 * | 5   | 3.3V supply                               | 3.3V          |
 * | 6   | LCD reset (+3.3V !)                       | 47 ⚡         |
 * | 7   | LCD backlight (+5V !)                     | 5V            |
 * | 8   | GND                                       | -             |
 * | 9   | Shift Register Clock Input (SCK)          | 52 (SPI SCK)  |
 * | 10  | Serial Data Input (SER IN)                | 51 (SPI MOSI) |
 * | 11  | Storage Register Clock Input (RCK)        | 48            |
 * | 12  | GND                                       | -             |
 * | 13  | LCD write  (+3.3V !)                      | 49 ⚡         |
 * | 14  | GND                                       | -             |
 *
 * ⚡ Require a 3.3v level-shifter, screen makes shadows and may be destroyed after long use.
 * 🔺 Require a 10k pull-up resistor wired between 3.3V and Arduino pin
 *
 *
 * @version 0.0  # x-release-please-version
 *
 * @copyright MIT license
 */

#include "Epson_PNL_CE02.h"
#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>

const char *buttonNames[8] = {
    "Right", "Ok", "Up", "Left", "Start", "Down", "Stop", "Home",
};

// UTILS
const char *buttonName(ButtonMask flag)
{
    const double bitPos = log(flag) / log(2);
    return buttonNames[(int)bitPos];
}

const bool isButtonPressed(byte sequence, ButtonMask flag)
{
    return (sequence & flag) != 0; // Check if key pressed 0000{key}000, key>0 if set
}

// CTOR
Epson_PNL_CE02::Epson_PNL_CE02(int oePin, int serOutPin, int powerButtonPin, int lcdResetPin, int clockPin,
                               int serInPin, int latchPin, int lcdWritePin)
{
    Epson_PNL_CE02::oePin = oePin;
    Epson_PNL_CE02::serOutPin = serOutPin;
    Epson_PNL_CE02::powerButtonPin = powerButtonPin;
    Epson_PNL_CE02::clockPin = clockPin;
    Epson_PNL_CE02::serInPin = serInPin;
    Epson_PNL_CE02::latchPin = latchPin;
}

// PUBLICS
void Epson_PNL_CE02::begin()
{
    pinMode(powerButtonPin, INPUT);

    pinMode(oePin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(serInPin, OUTPUT);
    pinMode(serOutPin, INPUT);

    SPI.begin();
    SPI.beginTransaction(SPISettings(F_CPU / 2, MSBFIRST, SPI_MODE0)); // Max SPI speed

    digitalWrite(oePin, LOW);
}

void Epson_PNL_CE02::extenderWrite(ExtenderPin pin, byte mode)
{
    bitWrite(buffer, pin, mode);
    synchronize();
}

void Epson_PNL_CE02::displayWrite(byte data)
{
    SPI.transfer(data);
}

byte Epson_PNL_CE02::readButtons()
{
    return synchronize();
}

bool Epson_PNL_CE02::isPowerButtonPressed()
{
    return digitalRead(powerButtonPin) == LOW;
}

// PRIVATES

byte Epson_PNL_CE02::synchronize()
{
    // STEP 1: Send control information (Power LED, LCD backlight, LCD CS, LCD D/C) through 74HC595
    digitalWrite(latchPin, LOW); // enables parallel inputs
    SPI.transfer(buffer);
    // STEP 2: Receive buttons inputs through 74LV165A
    digitalWrite(latchPin, HIGH); // disable parallel inputs and enable serial output

    return 0b11111111 ^ SPI.transfer(0xff); // read buttons (invert cause output is HIGH)
}
