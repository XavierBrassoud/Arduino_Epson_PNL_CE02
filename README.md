# Arduino - EPSON XP 520/530/540 Control Panel (PNL CE02) <!-- omit from toc -->

![logo](/docs/panel.jpg)

Library to repurposing the control panel (PNL CE02) of EPSON XP 520/530/540 printers from an Arduino.

- [Motivation](#motivation)
- [Installation](#installation)
- [Hardware requirements](#hardware-requirements)
- [Connecting the control panel](#connecting-the-control-panel)
- [Let's play!](#lets-play)
- [Using display](#using-display)
- [Library documentation](#library-documentation)
  - [Epson\_PNL\_CE02 Class](#epson_pnl_ce02-class)
    - [Constructor](#constructor)
    - [Functions](#functions)
  - [Utils](#utils)
  - [ButtonMask](#buttonmask)
  - [ExtenderPin](#extenderpin)
- [Credits](#credits)

## Motivation

> Nothing is lost, nothing is created, everything is transformed
>
> -- <cite>Antoine LAVOISIER</cite>

I don't like waste. So the day my printer stopped working, I put it aside until the day I needed a control panel. When this day came, I decided to embark on the reverse engineering of this control panel. Of course, there are many components compatible with Arduino. But you know... in an ecological approach and especially for the technical challenges, I rushed headlong! See [REVERSE.md](REVERSE.md) for details.

This control panel can now be recycled and interfaced with an Arduino.

Happy hacking!

## Installation

From Arduino:

From PlatformIO:

Direct download from source (ZIP bundle):

## Hardware requirements

  * Arduino (tested with Arduino MEGA 2560)
  * FPC 14 pin 1.0mm adapter (avoid pads destroyed by soldering)
  * Level shifter (5v to 3.3v)
  * Control panel (PNL CE02) of EPSON XP 520/530/540 printers (obviously)

## Connecting the control panel

| Pin | Purpose                            | MEGA 2560     |
| --- | ---------------------------------- | ------------- |
| 1   | 3-STATE Output Enable Input (OE)   | 45            |
| 2   | Serial Data Output (SER OUT)       | 50 (SPI MISO) |
| 3   | GND                                | GND           |
| 4   | Power button                       | 46 🔺         |
| 5   | 3.3V supply                        | 3.3V          |
| 6   | LCD reset                          | 47 ⚡         |
| 7   | LCD backlight (+5V !)              | 5V            |
| 8   | GND                                | -             |
| 9   | Shift Register Clock Input (SCK)   | 52 (SPI SCK)  |
| 10  | Serial Data Input (SER IN)         | 51 (SPI MOSI) |
| 11  | Storage Register Clock Input (RCK) | 48            |
| 12  | GND                                | -             |
| 13  | LCD write                          | 49 ⚡         |
| 14  | GND                                | -             |

**⚡** Require a 3.3v level-shifter, screen makes shadows and may be destroyed after long use.

**🔺** Require a 10k pull-up resistor wired between 3.3V and Arduino pin

<details>
  <summary>Connecting diagram for Arduino MEGA 2560</summary>

  ![connecting_diagram_mega_2560](/docs/connecting_diagram_mega_2560.png)
</details>

## Let's play!

[examples/buttons]([examples/buttons/buttons.ino]):
``` c++
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
    Serial.begin(9600);
    controlPanel.begin();
}

void loop()
{
    switch (controlPanel.readButtons())
    {
    case OK:
        Serial.println("Button OK pressed!");
        break;
    case OK | HOME:
        Serial.println("Button OK and button LEFT pressed!");
        break;
    }

    // Power button has a dedicated pin
    if (controlPanel.isPowerButtonPressed())
    {
        Serial.println("Button POWER pressed!");
    }

    delay(50);
}
```

See more usages in [`examples`](./examples/) folder.


## Using display

The control panel embed a GIANTPLUS ILI9163C screen driven by 8-bit parallel interface.

This library is not a GFX library. You can rely on [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) library derivatives,
such as the excellent [MCUFRIEND_kbv](https://github.com/prenticedavid/MCUFRIEND_kbv) library.

Here is an adaptation using [MCUFRIEND_kbv](https://github.com/prenticedavid/MCUFRIEND_kbv) library:
1. Download [MCUFRIEND_kbv v3](https://github.com/prenticedavid/MCUFRIEND_kbv)
2. Edit *MCUFRIEND_kbv/utility/mcufriend_shield.h*:
   1. Uncomment `#define USE_SPECIAL`
   2. Uncomment `#define SUPPORT_9163`
3. Edit *MCUFRIEND_kbv/MCUFRIEND_kbv.cpp*:
   1. Go to `#ifdef SUPPORT_9163` section
   2. Replace `*p16 = 160;` by `*p16 = 128;`
5. Edit *MCUFRIEND_kbv/utility/mcufriend_special.h*:
   1. Write to the top of mcufriend_special.h `#define USE_EPSON_PNL_CE02`
   2. Copy content from *extras/mcufriend_specials/<ARDUINO_TYPE>.h* file
   3. Paste to the SPECIAL definitions of *mcufriend_special.h*, somewhere between `#if` and `#else`
6. Your code requires:
   1. Definition for `Epson_PNL_CE02 controlPanel(...)`
   2. Turn display ON before INIT:
      ``` c++
      controlPanel.extenderWrite(LCD_BACKLIGHT, HIGH);
      tft.begin(0x9163);
      ```

[examples/display]([examples/display/display.ino]):
``` c++

#include <Epson_PNL_CE02.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

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

MCUFRIEND_kbv tft;

void setup()
{
    controlPanel.begin();

    // STEP 1: Turn display ON
    controlPanel.extenderWrite(LCD_BACKLIGHT, HIGH);

    // STEP 2: INIT display
    tft.begin(0x9163); // ILI9163C

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

```

## Library documentation


### Epson_PNL_CE02 Class

#### Constructor

| Parameter        | Description                                                   |
| ---------------- | ------------------------------------------------------------- |
| `oePin`          | Extender output enable pin. Unused.                           |
| `serOutPin`      | Serial output pin. Read value for buttons. (SPI MISO)         |
| `powerButtonPin` | Dedicated pin for the power button.                           |
| `lcdResetPin`    | Display reset pin.                                            |
| `clockPin`       | Shared clock pin for extender, buttons and display. (SPI SCK) |
| `serInPin`       | Serial input pin. Write value in extender. (SPI MOSI)         |
| `latchPin`       | Write extender selector.                                      |
| `lcdWritePin`    | Display write pin.                                            |

#### Functions

| Function                                         | Description                                                                                                           |
| ------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------- |
| `void begin()`                                   | Set pins directions and initialize SPI bus.                                                                           |
| `void extenderWrite(ExtenderPin pin, byte mode)` | Write a `HIGH` or a `LOW` value to the control panel extender pin (refer to [`ExtenderPin`](#extenderpin)).           |
| `void displayWrite(byte data)`                   | Write parallel data (D0-D7) to the display.                                                                           |
| `byte readButtons()`                             | Read current pressed buttons in 8-bit sequence (`0`: no pressed, `1`: pressed, refer to [`ButtonMask`](#buttonmask)). |
| `bool isPowerButtonPressed()`                    | Determine if the power button is pressed or not. The power button has a dedicated pin.                                |

### Utils

| Function                                               | Description                                                                                     |
| ------------------------------------------------------ | ----------------------------------------------------------------------------------------------- |
| `char[] buttonName(ButtonMask mask)`                   | Get button name (refer to [`ButtonMask`](#buttonmask)).                                         |
| `bool isButtonPressed(byte sequence, ButtonMask mask)` | Determines if a button is pressed in the 8-bit sequence (refer to [`ButtonMask`](#buttonmask)). |

### ButtonMask

| Button      | Mask                                  |
| ----------- | ------------------------------------- |
| **`HOME`**  | `00000001`                            |
| **`STOP`**  | `00000010`                            |
| **`DOWN`**  | `00000100`                            |
| **`START`** | `00001000`                            |
| **`LEFT`**  | `00010000`                            |
| **`UP`**    | `00100000`                            |
| **`OK`**    | `01000000`                            |
| **`RIGHT`** | `10000000`                            |
| ~~`POWER`~~ | The power button has a dedicated pin. |

### ExtenderPin

| Pin                 | Active | Description                                                |
| ------------------- | ------ | ---------------------------------------------------------- |
| **`POWER_LED`**     | `LOW`  | Control the state of the power led.                        |
| **`LCD_BACKLIGHT`** | `HIGH` | Control the state of the display backlight.                |
| **`LCD_CS`**        | `HIGH` | Control the state of the display CS pin (ChipSelect).      |
| **`LCD_DC`**        | `HIGH` | Control the state of the display D/C pin (Data / Command). |

## Credits

Thanks [@phooky](https://github.com/phooky) for your [inspiring guide](https://www.nycresistor.com/2022/01/18/repurposing-control-panel/)!

Examples uses:
 * [@mathertel](https://github.com/mathertel)'s [OneButton](https://github.com/mathertel/OneButton) library (BSD license).
 * [@prenticedavid](https://github.com/prenticedavid)'s [MCUFRIEND_kbv](https://github.com/prenticedavid/MCUFRIEND_kbv) library (BSD license).
 * [Adafruit](https://github.com/adafruit) team for [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) library (BSD license).


And... EPSON for the control panel 😉
