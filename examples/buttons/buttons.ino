/**
 * @file buttons.cpp
 * @author Xavier BRASSOUD (contact@xavierbrassoud.fr)
 * @brief A basic buttons playground using Epson_PNL_CE02 library.
 * Get an advanced usage of buttons in the OneButton sketch.
 * @version 1.0
 * @date 2023-12-22
 *
 * @copyright MIT license
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
 */

#define BAUD_RATE 115200

#include <Epson_PNL_CE02.h>

enum
{
    /* Control panel to Arduino pinout */
    SP_OE = 31,        // FFC 1
    SP_SER_OUT = 32,   // FFC 2
    POWER_BUTTON = 34, // FFC 4
    LCD_RESET = 36,    // FFC 6
    SP_CLK = 39,       // FFC 9
    SP_SER_IN = 40,    // FFC 10
    SP_RCK = 41,       // FFC 11
    LCD_WRITE = 43,    // FFC 13
};

Epson_PNL_CE02 controlPanel(SP_OE, SP_SER_OUT, POWER_BUTTON, LCD_RESET, SP_CLK, SP_SER_IN, SP_RCK, LCD_WRITE);

void setup()
{
    Serial.begin(BAUD_RATE);
}

void loop()
{
    // Synchronize data
    controlPanel.synchronize();

    switch (controlPanel.readButtons())
    {
    case OK:
        Serial.println("Button OK pressed !");
        break;
    case OK | HOME:
        Serial.println("Button OK and button LEFT pressed !");
        break;
    }

    // Power button has a dedicated pin
    if (controlPanel.isPowerButtonPressed())
    {
        Serial.println("POWER button pressed !");
    }

    delay(50);
}