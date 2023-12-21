/*!
 * @file Epson_PNL_CE02.h
 *
 * Library to repurposing the control panel (PNL CE02) of EPSON XP 520/530/540
 * printers from an Arduino.
 *
 * Designed specifically to work with the EPSON PNL CE02 board
 * ----> EPSON XP 520
 * ----> EPSON XP 530
 * ----> EPSON XP 540
 *
 * These displays use LV165 shift register for buttons, I2C to communicate.
 *
 * The FFC from the CPU to the control board has this pinout:
 * | Pin | Purpose                                   |
 * |-----|-------------------------------------------|
 * | 1   | 3-STATE Output Enable Input (OE)          |
 * | 2   | Serial Data Output (SER OUT)              |
 * | 3   | GND                                       |
 * | 4   | on/off btn                                |
 * | 5   | 3.3V supply                               |
 * | 6   | LCD UNKNOWN                               |
 * | 7   | LCD VIN UNKNOWN                           |
 * | 8   | GND                                       |
 * | 9   | Shift Register Clock Input (SCK)          |
 * | 10  | Serial Data Input (SER IN)                |
 * | 11  | Storage Register Clock Input (RCK)        |
 * | 12  | GND                                       |
 * | 13  | Unknown                                   |
 * | 14  | GND                                       |
 *
 *
 * Written by Xavier BRASSOUD.
 * MIT license
 */

#ifndef Epson_PNL_CE02_H
#define Epson_PNL_CE02_H

/*!
 * @brief Buttons byte mapping.
 */
enum Key
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

const char *keyNames[8] = {
    "Right",
    "Ok",
    "Up",
    "Left",
    "Start",
    "Down",
    "Stop",
    "Home",
};

/*!
 * @brief Get button name.
 *
 * @param key Buttons byte.
 * @return const char* button name.
 */
const char *keyName(Key key);

/*!
 * @brief Determines if a button is pressed in the sequence.
 *
 * @param keys buttons sequence.
 * @param key button pressed.
 * @return true
 * @return false
 */
const bool isKeyPressed(uint8_t keys, Key key);

class ControlPanel
{

public:
    /*int serOutPin() const { return serOutPin; }
    int clockPin() const { return clockPin; }
    int shiftLoadPin() const { return shiftLoadPin; }

    const unsigned long *keysTimers() { return keysTimers; }

    unsigned long debounceDelay() const { return debounceDelay; }
    unsigned long longPressDelay() const { return longPressDelay; }*/

    /*!
     * @brief Construct a new Control Panel object
     *
     * @param oePin
     * @param serOutPin
     * @param clockPin
     * @param serInPin
     * @param shiftLoadPin
     * @param debounceDelay
     * @param longPressDelay
     *
     * @example
     * ``` c++
     * const cp = ControlPanel(...);
     * cp.read();
     * switch(cp.readKeys()) {
     *  case Key.HOME:
     *      Serial.println("Key HOME pressed.");
     *      break;
     *  case Key.HOME | Key.OK:
     *      Serial.println("Key HOME and key OK pressed.");
     *      break;
     * }
     * ```
     */
    ControlPanel(int oePin, int serOutPin, int clockPin, int serInPin, int shiftLoadPin, unsigned long debounceDelay = 50, unsigned long longPressDelay = 2000);

    /*!
     * @brief Read current pressed keys in binary format of 1 byte (0: no pressed, 1: pressed).
     *
     * @return byte Keys currently pressed.
     */
    uint8_t readKeys() { return pressedKeys; }

    /*!
     * @brief Read last pressed keys in binary format of 1 byte (0: no pressed, 1: pressed).
     *
     * @return byte Keys pressed.
     */
    uint8_t readPressedKeys() { return shortPressedKeys | longPressedKeys; }

    /*!
     * @brief Read short pressed keys in binary format of 1 byte (0: no pressed, 1: pressed).
     *
     * @return byte
     */
    uint8_t readShortPressedKeys() { return shortPressedKeys; }

    /*!
     * @brief Read long pressed keys in binary format of 1 byte (0: no pressed, 1: pressed).
     *
     * @return byte
     */
    uint8_t readLongPressedKeys() { return longPressedKeys; }

    /*!
     * @brief Read inputs loop.
     *
     */
    void read();

private:
    unsigned int serOutPin, clockPin, shiftLoadPin;
    unsigned long debounceDelay, longPressDelay, keysTimers[8];
    uint8_t pressedKeys, longPressedKeys, shortPressedKeys = 0b00000000;

    void keysDispatcher(uint8_t keys);

};

#endif //  Epson_PNL_CE02_H