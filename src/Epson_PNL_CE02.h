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

#ifndef Epson_PNL_CE02_H
#define Epson_PNL_CE02_H

#include <stdint.h>

/**
 * @brief Buttons 8-bit mapping.
 */
enum ButtonFlag
{
    RIGHT = 0b00000001, // 0b11111110
    OK = 0b00000010,    // 0b11111101
    UP = 0b00000100,    // 0b11111011
    LEFT = 0b00001000,  // 0b11110111
    START = 0b00010000, // 0b11101111
    DOWN = 0b00100000,  // 0b11011111
    STOP = 0b01000000,  // 0b10111111
    HOME = 0b10000000,  // 0b01111111
};

/**
 * @brief Get button name.
 *
 * @param flag 8-bit buttons sequence.
 * @return const char* button name.
 */
const char *buttonName(ButtonFlag flag);

/**
 * @brief Determines if a button is pressed in the 8-bit sequence.
 *
 * @param sequence 8-bit buttons sequence.
 * @param flag button pressed.
 * @return true
 * @return false
 */
const bool isButtonPressed(uint8_t sequence, ButtonFlag flag);

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
     * @param oePin 
     * @param serOutPin 
     * @param powerButtonPin 
     * @param lcdResetPin 
     * @param clockPin 
     * @param serInPin 
     * @param shiftLoadPin 
     * @param lcdWritePin 
     */
    Epson_PNL_CE02(int oePin, int serOutPin, int powerButtonPin, int lcdResetPin, int clockPin, int serInPin, int shiftLoadPin, int lcdWritePin);

    /**
     * @brief Read and write to shift registers that control buttons, power led and display.
     * Called each time a refresh is needed.
     * 
     */
    void update();

    /**
     * @brief Read current pressed buttons in binary format of 8 bits (0: no pressed, 1: pressed).
     * Use ButtonFlag to determine witch button is pressed.
     * Depends on update().
     *
     * @return byte Sequence of buttons currently pressed.
     */
    uint8_t readButtons() { return buttonsSequence;};

    /**
     * @brief Determine if the power button is pressed or not. The power button has a dedicated pin.
     * 
     * @return true 
     * @return false 
     */
    bool isPowerButtonPressed();

    /**
     * @brief Control the state of the power led (HIGH: on, LOW: off).
     * Depends on update().
     */
    void writePowerLed(uint8_t state) { ledState = state; };

private:
    unsigned int oePin, serOutPin, powerButtonPin, lcdResetPin, clockPin, serInPin, shiftLoadPin, lcdWritePin;
    uint8_t buttonsSequence;
    uint8_t ledState;
};

#endif //  Epson_PNL_CE02_H