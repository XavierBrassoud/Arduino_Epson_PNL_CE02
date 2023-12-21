//**************************************************************//
//  Name    : EPSON XP 520/530/540 Control Panel dialer.         //
//  Author  : Xavier BRASSOUD                                    //
//  Date    : 11 Dec, 2023                                       //
//  Version : 1.0                                                //
//  Notes   : Code for repurposing printer control panel of      //
//            EPSON XP 520/530/540 control from Arduino.         //
//**************************************************************//

#define BAUD_RATE 115200

#include <Arduino.h>
#include <control_panel.h>

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


enum {
  /* Serial Parallel Shifting Out (button communication) */
  SP_OE        = 31,  // FFC 1
  SP_SER_OUT   = 32,  // FFC 2
  SP_CLK       = 39,  // FFC 9
  SP_SER_IN    = 40,  // FFC 10
  SP_RCK      = 41,  // FFC 11
};

ControlPanel cp(-1, SP_SER_OUT, SP_CLK, -1, SP_RCK);

// function declarations
void printKeys(byte keys, const char *mode = NULL);

void setup()
{
  Serial.begin(BAUD_RATE);
}

void loop()
{
  cp.read();

  byte cur = cp.readKeys();
  //printKeys(cur, "currently");

  byte pressed = cp.readPressedKeys();
  printKeys(pressed);
  // Serial.print("shrt: ");
  // Serial.println(shrt, BIN);
  /*switch (pressed)
  {
  case OK:
    Serial.println("Key OK pressed !");
    break;
  case OK | LEFT:
    Serial.println("Key OK and key LEFT pressed !");
    break;
  }*/

  byte shrt = cp.readShortPressedKeys();
  printKeys(shrt, "shortly");

  byte lng = cp.readLongPressedKeys();
  printKeys(lng, "longly");

  delay(50);
}

void printKeys(byte keys, const char *mode)
{
  if (keys != 0b00000000)
  {
    Serial.print("Key(s) ");
    for (int i = 0; i < 8; i++)
    {
      Key key = Key(0b00000000 | (1 << i));
      if (isKeyPressed(keys, key))
      {
        Serial.print(keyName(key));
        Serial.print(" ");
      }
    }
    if (mode != NULL)
    {
      Serial.print(mode);
      Serial.print(" ");
    }
    Serial.println("pressed");
  }
}
