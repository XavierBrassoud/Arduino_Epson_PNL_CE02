/**
 * @file display.ino
 * @author Xavier BRASSOUD (contact@xavierbrassoud.fr)
 * @brief Show things on the display using the Epson_PNL_CE02 library.
 * The control panel screen is a GIANTPLUS ILI9163C model driven by 8-bit
 * parallel interface.
 * This sketch running only on Arduino MEGA 2560.
 * @version 1.0
 * @date 2023-12-28
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
 *
 * 
 * Adaptation:
 * 1. Download [MCUFRIEND_kbv v3](https://github.com/prenticedavid/MCUFRIEND_kbv)
 * 2. Edit *MCUFRIEND_kbv/utility/mcufriend_shield.h*:
 *    1. Uncomment `#define USE_SPECIAL`
 *    2. Uncomment `#define SUPPORT_9163`
 * 3. Edit *MCUFRIEND_kbv/MCUFRIEND_kbv.cpp*:
 *    1. Go to `#ifdef SUPPORT_9163` section
 *    2. Replace `*p16 = 160;` by `*p16 = 128;`
 * 5. Edit *MCUFRIEND_kbv/utility/mcufriend_special.h*:
 *    1. Write to the top of mcufriend_special.h `#define USE_EPSON_PNL_CE02`
 *    2. Copy content from *extras/mcufriend_specials/<ARDUINO_TYPE>.h* file
 *    3. Paste to the SPECIAL definitions of *mcufriend_special.h*, somewhere between `#if` and `#else`
 * 6. Your code requires:
 *    1. Definition for `Epson_PNL_CE02 controlPanel(...)`
 *    2. Turn display ON before INIT:
 *       ``` c++
 *       controlPanel.extenderWrite(LCD_BACKLIGHT, HIGH);
 *       tft.begin(0x9163);
 * 
 */

#include <Epson_PNL_CE02.h>

/****************************** Epson_PNL_CE02 *******************************/
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

/******************************* MCUFRIEND_kbv *******************************/

// install the Adafruit_GFX library via Library Manager
// if using the Arduino IDE, click here: http://librarymanager#Adafruit_GFX
#include <Adafruit_GFX.h>

// install the MCUFRIEND_kbv library via Library Manager
// if using the Arduino IDE, click here: http://librarymanager#MCUFRIEND_kbv
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv tft;

/*********************************** MAIN ************************************/

void setup()
{
    controlPanel.begin();

    // STEP 1: Turn display ON
    controlPanel.extenderWrite(LCD_BACKLIGHT, HIGH);

    // STEP 2: INIT display
    tft.begin(0x9163); // Force ILI9163C as the control panel wired the display in write-only mode

    // STEP 3: Use display
    tft.fillScreen(TFT_RED);
    tft.setCursor(5, 5);
    tft.setTextColor(TFT_BLACK);
    tft.print("Hello World");
}

void loop()
{
    delay(1000);
}
