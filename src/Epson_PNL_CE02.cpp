/**
 * @file Epson_PNL_CE02.cpp
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

// UTILS
// cppcheck-suppress unusedFunction
const char *buttonName(ButtonMask mask)
{
    switch (mask)
    {
    case ButtonMask::RIGHT:
        return "Right";
    case ButtonMask::OK:
        return "Ok";
    case ButtonMask::UP:
        return "Up";
    case ButtonMask::LEFT:
        return "Left";
    case ButtonMask::START:
        return "Start";
    case ButtonMask::DOWN:
        return "Down";
    case ButtonMask::STOP:
        return "Stop";
    case ButtonMask::HOME:
        return "Home";
    }
    return "Unknown";
}

// cppcheck-suppress unusedFunction
bool isButtonPressed(byte sequence, ButtonMask mask)
{
    return (sequence & (byte)mask) != 0; // Check if key pressed 0000{key}000, key>0 if set
}

// CTOR
Epson_PNL_CE02::Epson_PNL_CE02(Epson_PNL_CE02_Pinout *pPinout) : pins(pPinout) {}

// PUBLICS
void Epson_PNL_CE02::begin() const
{
    pinMode(pins->power_button, INPUT);

    pinMode(pins->extender_oe, OUTPUT);
    pinMode(pins->latch, OUTPUT);
    pinMode(pins->clock, OUTPUT);
    pinMode(pins->serial_in, OUTPUT);
    pinMode(pins->serial_out, INPUT);

    SPIClass::begin();
    SPIClass::beginTransaction(SPISettings(F_CPU / 2, MSBFIRST, SPI_MODE0)); // Max SPI speed

    digitalWrite(pins->extender_oe, LOW);
}

// cppcheck-suppress unusedFunction
void Epson_PNL_CE02::extenderWrite(ExtenderPin pin, byte mode)
{
    bitWrite(buffer, (byte)pin, mode);
    synchronize();
}

// cppcheck-suppress unusedFunction
void Epson_PNL_CE02::displayWrite(byte data)
{
    SPIClass::transfer(data);
}

byte Epson_PNL_CE02::readButtons()
{
    return synchronize();
}

// cppcheck-suppress unusedFunction
bool Epson_PNL_CE02::isPowerButtonPressed() const
{
    return digitalRead(pins->power_button) == LOW;
}

// PRIVATES
byte Epson_PNL_CE02::synchronize() const
{
    // STEP 1: Send control information (Power LED, LCD backlight, LCD CS, LCD D/C) through 74HC595
    digitalWrite(pins->latch, LOW); // enables parallel inputs
    SPIClass::transfer(buffer);
    // STEP 2: Receive buttons inputs through 74LV165A
    digitalWrite(pins->latch, HIGH); // disable parallel inputs and enable serial output

    const byte FULL_MASK = 0b11111111;
    return FULL_MASK ^ SPIClass::transfer(FULL_MASK); // read buttons (invert cause output is HIGH)
}
