/**
 * @file OneButton.cpp
 * @author Xavier BRASSOUD (contact@xavierbrassoud.fr)
 * @brief Buttons playground using Epson_PNL_CE02 library with OneButton library.
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

// install the OneButton library via Library Manager
// if using the Arduino IDE, click here: http://librarymanager#OneButton
#include <OneButton.h>

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

void togglePowerLed()
{
  Serial.println("Click: power long - toggle power LED");

  // toggle power led (switch on or off)
  controlPanel.writePowerLed(!controlPanel.readPowerLed());
}

// function declarations
void setup()
{
  Serial.begin(BAUD_RATE);

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
  homeButton->attachDoubleClick(printClick, (void *)"home x2");
  homeButton->attachLongPressStop(printClick, (void *)"home long");

  // Parallel buttons events
  homeOkButton->attachClick(printClick, (void *)"home + ok");

  powerButton.attachClick(printClick, (void *)"power");
  powerButton.attachLongPressStop(togglePowerLed);
}

void loop()
{
  // Get refreshed data
  controlPanel.synchronize();

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
