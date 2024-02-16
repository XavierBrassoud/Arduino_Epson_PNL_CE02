/**
 * @file blink.ino
 * @author Xavier BRASSOUD (contact@xavierbrassoud.fr)
 * @brief Blinks the power LED using the Epson_PNL_CE02 library.
 * @version 1.0
 *
 * @copyright MIT license
 *
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
 */

#include <Epson_PNL_CE02.h>

enum
{
    /* Control panel to Arduino pinout */
    EXTENDER_OE = 45,  // FFC 1
    SERIAL_OUT = 50,   // SPI MISO / FFC 2
    POWER_BUTTON = 46, // FFC 4
    LCD_RESET = 47,    // FFC 6
    CLOCK = 52,        // SPI SCK / FFC 9
    SERIAL_IN = 51,    // SPI MOSI / FFC 10
    LATCH = 48,        // FFC 11
    LCD_WRITE = 49,    // FFC 13
};

Epson_PNL_CE02 controlPanel(EXTENDER_OE, SERIAL_OUT, POWER_BUTTON, LCD_RESET, CLOCK, SERIAL_IN, LATCH, LCD_WRITE);

void setup()
{
    controlPanel.begin();
}

void loop()
{
    // turns ON power LED (active LOW)
    controlPanel.extenderWrite(POWER_LED, LOW);

    delay(1000);

    // turns OFF power LED
    controlPanel.extenderWrite(POWER_LED, HIGH);

    delay(1000);
}