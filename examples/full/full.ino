/**
 * @file full.ino
 * @author Xavier BRASSOUD (contact@xavierbrassoud.fr)
 * @brief Full playground using Epson_PNL_CE02 library with OneButton library
 * and MCUFRIEND_kbv. See "display" example to wire the display correctly.
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
 * 
 * Fix OneButton issue: https://github.com/mathertel/OneButton/issues/136#issuecomment-1891117014
 */

#if defined(ARDUINO_ARCH_AVR)
    #define BAUD_RATE 115200
#else
    #define BAUD_RATE 9600
#endif

// install the OneButton library via Library Manager
// if using the Arduino IDE, click here: http://librarymanager#OneButton
#include <OneButton.h>

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

/******************************* MCUFRIEND_kbv *******************************/

// install the Adafruit_GFX library via Library Manager
// if using the Arduino IDE, click here: http://librarymanager#Adafruit_GFX
#include <Adafruit_GFX.h>

// install the MCUFRIEND_kbv library via Library Manager
// if using the Arduino IDE, click here: http://librarymanager#MCUFRIEND_kbv
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv tft;

/*********************************** MAIN ************************************/

// Single buttons events
OneButton *rightButton;
OneButton *okButton;
OneButton *upButton;
OneButton *leftButton;
OneButton *startButton;
OneButton *downButton;
OneButton *stopButton;
OneButton *homeButton;

// Parallel buttons events
OneButton *homeOkButton;

// Power button has a dedicated pin
OneButton powerButton(POWER_BUTTON);

void printClick(void *button)
{
  Serial.print("Click: ");
  Serial.println((char *)button);
}

void colorClick(void *color)
{
  tft.fillScreen((uint16_t)color);
}

byte powerState = HIGH;
void togglePower()
{
  Serial.println("Click: power long - toggle power");

  // reverse power (turns off when on, on when off)
  powerState = !powerState;

  // send new power LED state.
  controlPanel.extenderWrite(POWER_LED, !powerState);

  // send new power SCREEN state.
  controlPanel.extenderWrite(LCD_BACKLIGHT, powerState);
}

// function declarations
void setup()
{
  Serial.begin(BAUD_RATE);
  controlPanel.begin();

  controlPanel.extenderWrite(LCD_BACKLIGHT, HIGH);
  tft.begin(0x9163); // ILI9163C

  rightButton = new OneButton();
  okButton = new OneButton();
  upButton = new OneButton();
  leftButton = new OneButton();
  startButton = new OneButton();
  downButton = new OneButton();
  stopButton = new OneButton();
  homeButton = new OneButton();

  homeOkButton = new OneButton();

  // Single buttons events
  rightButton->attachClick(printClick, (void *)"right");
  okButton->attachClick(printClick, (void *)"ok");
  upButton->attachClick(printClick, (void *)"up");
  leftButton->attachClick(printClick, (void *)"left");
  startButton->attachClick(printClick, (void *)"start");
  downButton->attachClick(printClick, (void *)"down");
  stopButton->attachClick(printClick, (void *)"stop");
  homeButton->attachClick(printClick, (void *)"home");
  
  rightButton->attachClick(colorClick, (void *)TFT_LIGHTGREY);
  okButton->attachClick(colorClick, (void *)TFT_BLUE);
  upButton->attachClick(colorClick, (void *)TFT_ORANGE);
  leftButton->attachClick(colorClick, (void *)TFT_MAGENTA);
  startButton->attachClick(colorClick, (void *)TFT_GREEN);
  downButton->attachClick(colorClick, (void *)TFT_PURPLE);
  stopButton->attachClick(colorClick, (void *)TFT_RED);
  homeButton->attachClick(colorClick, (void *)TFT_YELLOW);
  
  homeButton->attachDoubleClick(printClick, (void *)"home x2");
  homeButton->attachLongPressStop(printClick, (void *)"home long");

  // Parallel buttons events
  homeOkButton->attachClick(printClick, (void *)"home + ok");

  powerButton.attachClick(printClick, (void *)"power");
  powerButton.attachLongPressStop(togglePower);
}

void loop()
{
  byte buttonsSequence = controlPanel.readButtons();

  rightButton->tick(isButtonPressed(buttonsSequence, RIGHT));
  okButton->tick(isButtonPressed(buttonsSequence, OK));
  upButton->tick(isButtonPressed(buttonsSequence, UP));
  leftButton->tick(isButtonPressed(buttonsSequence, LEFT));
  startButton->tick(isButtonPressed(buttonsSequence, START));
  downButton->tick(isButtonPressed(buttonsSequence, DOWN));
  stopButton->tick(isButtonPressed(buttonsSequence, STOP));
  homeButton->tick(isButtonPressed(buttonsSequence, HOME));

  // Custom isButtonPressed bits manipulation
  homeOkButton->tick(isButtonPressed(buttonsSequence, HOME) && isButtonPressed(buttonsSequence, OK));

  powerButton.tick();

  delay(50);
}
