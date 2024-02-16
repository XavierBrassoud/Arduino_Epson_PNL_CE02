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
 * @version 0.0
 * @date 2023-12-22
 *
 * @copyright MIT license
 */

#ifndef Epson_PNL_CE02_H
#define Epson_PNL_CE02_H

#include <Arduino.h>

/**
 * @brief Buttons 8-bit mapping.
 */
enum ButtonMask
{
    RIGHT = 0b10000000, // 0b11111110
    OK = 0b01000000,    // 0b11111101
    UP = 0b00100000,    // 0b11111011
    LEFT = 0b00010000,  // 0b11110111
    START = 0b00001000, // 0b11101111
    DOWN = 0b00000100,  // 0b11011111
    STOP = 0b00000010,  // 0b10111111
    HOME = 0b00000001,  // 0b01111111
};

/**
 * @brief Shift register pins (VHC595)
 */
enum ExtenderPin
{
    /**
     * @brief Control the state of the power led (active LOW).
     */
    POWER_LED = 7,

    /**
     * @brief Control the state of the display backlight (active HIGH).
     */
    LCD_BACKLIGHT = 6,

    /**
     * @brief Control the state of the display CS pin (ChipSelect, active HIGH).
     */
    LCD_CS = 5,

    /**
     * @brief Control the state of the display D/C pin (Data / Command, active HIGH).
     */
    LCD_DC = 4
};

/**
 * @brief Get button name.
 *
 * @param mask 8-bit buttons sequence.
 * @return const char* button name.
 */
const char *buttonName(ButtonMask mask);

/**
 * @brief Determines if a button is pressed in the 8-bit sequence.
 *
 * @param sequence 8-bit buttons sequence.
 * @param mask button pressed.
 * @return true
 * @return false
 */
const bool isButtonPressed(byte sequence, ButtonMask mask);

/**
 * @brief Board class controller.
 *
 * @example
 * ``` c++
 * const cp = Epson_PNL_CE02(...);
 * switch(cp.readButtons()) {
 *  case Key.HOME:
 *      Serial.println("Button HOME pressed.");
 *      break;
 *  case Key.HOME | Key.OK:
 *      Serial.println("Button HOME and button OK pressed.");
 *      break;
 * }
 * ```
 */
class Epson_PNL_CE02
{

public:
    /**
     * @brief Construct a new Epson_PNL_CE02 object
     *
     * @param oePin Extender output enable pin. Unused.
     * @param serOutPin Serial output pin. Read value for buttons. (SPI MISO)
     * @param powerButtonPin Dedicated pin for the power button.
     * @param lcdResetPin Display reset pin.
     * @param clockPin Shared clock pin for extender, buttons and display. (SPI SCK)
     * @param serInPin Serial input pin. Write value in extender. (SPI MOSI)
     * @param latchPin Write extender selector.
     * @param lcdWritePin Display write pin.
     */
    Epson_PNL_CE02(int oePin, int serOutPin, int powerButtonPin, int lcdResetPin, int clockPin, int serInPin, int latchPin, int lcdWritePin);

    /**
     * @brief Set pins directions and initialize SPI bus.
     */
    void begin();

    /**
     * @brief Send a HIGH or a LOW value to the control panel extender pin (refer to ExtenderPin).
     *
     * @param pin shift register pin
     * @param mode HIGH / LOW
     */
    void extenderWrite(ExtenderPin pin, byte mode);

    /**
     * @brief Send parallel data (D0-D7) to the TFT display through the non latched 74HC164.
     *
     * @param data parallel data (D0-D7) byte
     */
    void displayWrite(byte data);

    /**
     * @brief Read current pressed buttons in 8-bit sequence (`0`: no pressed, `1`: pressed).
     * Use ButtonMask to determine witch button is pressed.
     *
     * @return byte Current pressed buttons in 8-bit sequence
     */
    byte readButtons();

    /**
     * @brief Determine if the power button is pressed or not. The power button has a dedicated pin.
     * Require a pull-up resistor (10K).
     *
     * @return true
     * @return false
     */
    bool isPowerButtonPressed();

private:
    unsigned int oePin, serOutPin, powerButtonPin, clockPin, serInPin, latchPin;
    byte buttonsSequence; // SERIAL OUT 74LV165A - All buttons except power
    byte buffer;          // SERIAL IN 74HC595 - Control panel extender (refer to ExtenderPin)

    /**
     * @brief Read and write to shift registers that control buttons, power led and display.
     * Called each time a refresh is needed.
     *
     * @return byte Current pressed buttons in 8-bit sequence
     */
    byte synchronize();
};

#endif //  Epson_PNL_CE02_H