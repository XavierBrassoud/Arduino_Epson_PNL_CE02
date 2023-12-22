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
 */

#define BAUD_RATE 115200


#include <Epson_PNL_CE02.h>

enum
{
    /* Serial Parallel Shifting Out (button communication) */
    SP_OE = 31,      // FFC 1
    SP_SER_OUT = 32, // FFC 2
    SP_CLK = 39,     // FFC 9
    SP_SER_IN = 40,  // FFC 10
    SP_RCK = 41,     // FFC 11
};

Epson_PNL_CE02 controlPanel(-1, SP_SER_OUT, SP_CLK, -1, SP_RCK);

void setup()
{
    Serial.begin(BAUD_RATE);
}

void loop()
{
    switch (controlPanel.readButtons())
    {
    case OK:
        Serial.println("Key OK pressed !");
        break;
    case OK | HOME:
        Serial.println("Key OK and key LEFT pressed !");
        break;
    }
}