//**************************************************************//
//  Name    : EPSON XP 520/530/540 Control Panel dialer.         //
//  Author  : Xavier BRASSOUD                                    //
//  Date    : 11 Dec, 2023                                       //
//  Version : 1.0                                                //
//  Notes   : Code for repurposing printer control panel of      //
//            EPSON XP 520/530/540 control from Arduino.         //
//**************************************************************//
//
//
/* The FFC from the CPU to the control board has this pinout: */
/* | Pin | Purpose                                   | */
/* |-----|-------------------------------------------| */
/* | 1   | 3-STATE Output Enable Input (OE)          | */
/* | 2   | Serial Data Output (SER OUT)              | */
/* | 3   | GND                                       | */
/* | 4   | on/off btn                                | */
/* | 5   | 3.3V supply                               | */
/* | 6   | LCD UNKNOWN                               | */
/* | 7   | LCD VIN UNKNOWN                           | */
/* | 8   | GND                                       | */
/* | 9   | Shift Register Clock Input (SCK)          | */
/* | 10  | Serial Data Input (SER IN)                | */
/* | 11  | Storage Register Clock Input (RCK)        | */
/* | 12  | GND                                       | */
/* | 13  | Unknown                                   | */
/* | 14  | GND                                       | */

#define BAUD_RATE 115200

#include <Arduino.h>
#include <OneButton.h>

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

void printClick(void *button)
{
  Serial.print("Click: ");
  Serial.println((char *)button);
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

  delay(50);
}
