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
 * @version 1.0.1  # x-release-please-version
 *
 * @copyright MIT license
 */

#ifndef EPSON_PNL_CE02_H
#define EPSON_PNL_CE02_H

#include <Arduino.h>

/**
 * @brief Buttons 8-bit mapping.
 */
enum class ButtonMask : byte // NOLINT(readability-identifier-naming): Bug clangtidy v15: enum detected as variable
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
enum class ExtenderPin : byte // NOLINT(readability-identifier-naming): Bug clangtidy v15: enum detected as variable
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
     * @brief Control the state of the display ChipSelect pin (active HIGH).
     */
    LCD_CS = 5,

    /**
     * @brief Control the state of the display Data/Command pin (active HIGH).
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
bool isButtonPressed(byte sequence, ButtonMask mask);

/**
 * @brief Control panel to Arduino pinout.
 *
 */
struct Epson_PNL_CE02_Pinout
{

    /**
     * @brief [UNUSED] Extender output enable pin.
     * Pin #1
     */
    const byte EXTENDER_OE;

    /**
     * @brief Serial output pin. Read value for buttons. (SPI MISO)
     * Pin #2
     */
    const byte SERIAL_OUT;

    /**
     * @brief Dedicated pin for the power button.
     * Pin #4
     */
    const byte POWER_BUTTON;

    /**
     * @brief Display reset pin. Refer to "Using display" section.
     * Pin #6
     */
    const byte LCD_RESET;

    /**
     * @brief Shared clock pin for extender, buttons and display. (SPI SCK)
     * Pin #9
     */
    const byte CLOCK;

    /**
     * @brief Serial input pin. Write value in extender. (SPI MOSI)
     * Pin #10
     */
    const byte SERIAL_IN;

    /**
     * @brief Write extender selector.
     * Pin #11
     */
    const byte LATCH;

    /**
     * @brief Display write pin. Refer to "Using display" section.
     * Pin #13
     */
    const byte LCD_WRITE;
};

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
class Epson_PNL_CE02 // NOLINT(readability-identifier-naming): Exception to follow common Arduino Library style naming
{

  public:
    /**
     * @brief Construct a new Epson_PNL_CE02 object
     *
     * @param pPinout Reference to Epson_PNL_CE02_Pinout structure.
     */
    explicit Epson_PNL_CE02(Epson_PNL_CE02_Pinout *pPinout);

    /**
     * @brief Set pins directions and initialize SPI bus.
     */
    void begin() const;

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
    static void displayWrite(byte data);

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
    bool isPowerButtonPressed() const;

  private:
    Epson_PNL_CE02_Pinout *pins;
    byte buffer{0b0}; // SERIAL IN 74HC595 - Control panel extender (refer to ExtenderPin)

    /**
     * @brief Read and write to shift registers that control buttons, power led and display.
     * Called each time a refresh is needed.
     *
     * @return byte Current pressed buttons in 8-bit sequence
     */
    byte synchronize() const;
};

#endif // Epson_PNL_CE02_H
