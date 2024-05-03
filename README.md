# Arduino - EPSON XP 520/530/540 Control Panel (PNL CE02) <!-- omit from toc -->

![logo](https://github.com/XavierBrassoud/Arduino_Epson_PNL_CE02/raw/v1.0.0/docs/panel.jpg)

Library to repurposing the control panel (PNL CE02) of EPSON XP 520/530/540 printers from an Arduino.

- [Motivation](#motivation)
- [Installation](#installation)
- [Hardware requirements](#hardware-requirements)
- [Connecting the control panel](#connecting-the-control-panel)
- [Let's play!](#lets-play)
- [Using display](#using-display)
- [Library documentation](#library-documentation)
  - [Epson\_PNL\_CE02\_Pinout Struct](#epson_pnl_ce02_pinout-struct)
  - [Epson\_PNL\_CE02 Class](#epson_pnl_ce02-class)
    - [Constructor](#constructor)
    - [Functions](#functions)
  - [Utils](#utils)
  - [ButtonMask](#buttonmask)
  - [ExtenderPin](#extenderpin)
- [Credits](#credits)
- [Like this project? ❤️](#like-this-project-️)

## Motivation

> Nothing is lost, nothing is created, everything is transformed
>
> -- <cite>Antoine LAVOISIER</cite>

I don't like waste. So the day my printer stopped working, I put it aside until the day I needed a control panel. When this day came, I decided to embark on the reverse engineering of this control panel. Of course, there are many components compatible with Arduino. But you know... in an ecological approach and especially for the technical challenges, I rushed headlong! See [REVERSE.md](REVERSE.md) for details.

This control panel can now be recycled and interfaced with an Arduino.

Happy hacking!

## Installation

 * **Arduino IDE**: Search for "[***Epson_PNL_CE02***](https://www.arduino.cc/reference/en/libraries/epson_pnl_ce02/)" in the built-in Arduino IDE Library manager ([details](https://www.arduino.cc/en/guide/libraries))

 * **PlatformIO**: Add [`XavierBassoud/Epson_PNL_CE02`](https://registry.platformio.org/libraries/xavierbrassoud/Epson_PNL_CE02) in the `lib_deps` section of your **platformio.ini** file ([details](https://docs.platformio.org/en/latest/core/userguide/pkg/cmd_install.html#cmd-pkg-install))

 * Direct download from source: [Clone this repository](https://github.com/XavierBrassoud/Arduino_Epson_PNL_CE02.git) into Arduino/Libraries

## Hardware requirements

  * [Arduino](https://amzn.to/3y5obqz) (tested with Arduino MEGA 2560)
  * [FPC 14 pin 1.0mm adapter](https://amzn.to/3wiy5EJ) (avoid pads destroyed by soldering)
  * [Level shifter](https://amzn.to/3Qrx6ZL) (5v to 3.3v)
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

  ![connecting_diagram_mega_2560](https://github.com/XavierBrassoud/Arduino_Epson_PNL_CE02/raw/v1.0.0/docs/connecting_diagram_mega_2560.png)
</details>

## Let's play!

[examples/buttons]([examples/buttons/buttons.ino]):
``` c++
#include <Epson_PNL_CE02.h>

Epson_PNL_CE02_Pinout pinout = {
    /* Control panel to Arduino pinout */
    .EXTENDER_OE = 45,  // FFC 1
    .SERIAL_OUT = 50,   // SPI MISO / FFC 2
    .POWER_BUTTON = 46, // FFC 4
    .LCD_RESET = 47,    // FFC 6
    .CLOCK = 52,        // SPI SCK / FFC 9
    .SERIAL_IN = 51,    // SPI MOSI / FFC 10
    .LATCH = 48,        // FFC 11
    .LCD_WRITE = 49,    // FFC 13
};

Epson_PNL_CE02 controlPanel(&pinout);

const byte OK = static_cast<byte>(ButtonMask::OK);
const byte HOME = static_cast<byte>(ButtonMask::HOME);

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
        Serial.println("Button OK and button HOME pressed!");
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
1. Locate your [MCUFRIEND_kbv v3](https://github.com/prenticedavid/MCUFRIEND_kbv) library:
   1. [PlatformIO](https://platformio.org/) users: directly edit in `.pio/libdeps/<your_project>/MCUFRIEND_kbv` **[⚠️ Non-persistent]** — or — [`Override package files`](https://docs.platformio.org/en/stable/scripting/examples/override_package_files.html).
   2. Other users: download [MCUFRIEND_kbv v3](https://github.com/prenticedavid/MCUFRIEND_kbv)
2. Edit *MCUFRIEND_kbv/utility/mcufriend_shield.h*:
   1. Uncomment `#define USE_SPECIAL`
3. Edit *MCUFRIEND_kbv/MCUFRIEND_kbv.cpp*:
   1. Uncomment `#define SUPPORT_9163`
   2. Go to `#ifdef SUPPORT_9163` section
   3. Replace `*p16 = 160;` by `*p16 = 128;`
4. Edit *MCUFRIEND_kbv/utility/mcufriend_special.h*:
   1. Write to the top of mcufriend_special.h `#define USE_EPSON_PNL_CE02`
   2. Copy content from *extras/mcufriend_specials/<ARDUINO_TYPE>.h* file
   3. Paste to the SPECIAL definitions of *mcufriend_special.h*, somewhere between `#if` and `#else`
5. Your code requires:
   1. Definition for `Epson_PNL_CE02 controlPanel(...)`
   2. Turn display ON before INIT:
      ``` c++
      controlPanel.extenderWrite(ExtenderPin::LCD_BACKLIGHT, HIGH);
      tft.begin(0x9163);
      ```

`examples/display` and `examples/full` depend on the adaptation above.

[examples/display]([examples/display/display.ino]):
``` c++

#include <Epson_PNL_CE02.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

Epson_PNL_CE02_Pinout pinout = {
    /* Control panel to Arduino pinout */
    .EXTENDER_OE = 45,  // FFC 1
    .SERIAL_OUT = 50,   // SPI MISO / FFC 2
    .POWER_BUTTON = 46, // FFC 4
    .LCD_RESET = 47,    // FFC 6
    .CLOCK = 52,        // SPI SCK / FFC 9
    .SERIAL_IN = 51,    // SPI MOSI / FFC 10
    .LATCH = 48,        // FFC 11
    .LCD_WRITE = 49,    // FFC 13
};

Epson_PNL_CE02 controlPanel(&pinout);

MCUFRIEND_kbv tft;

void setup()
{
    controlPanel.begin();

    // STEP 1: Turn display ON
    controlPanel.extenderWrite(ExtenderPin::LCD_BACKLIGHT, HIGH);

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

```

## Library documentation

### Epson_PNL_CE02_Pinout Struct

| Member           | Pin | Description                                                            |
| ---------------- | --- | ---------------------------------------------------------------------- |
| `EXTENDER_OE`    | #1  | Extender output enable pin. Unused.                                    |
| `SERIAL_OUT`     | #2  | Serial output pin. Read value for buttons. (SPI MISO)                  |
| `POWER_BUTTON`   | #4  | Dedicated pin for the power button.                                    |
| `LCD_RESET`      | #6  | Display reset pin. Refer to ["Using display"](#using-display) section. |
| `CLOCK`          | #9  | Shared clock pin for extender, buttons and display. (SPI SCK)          |
| `SERIAL_IN`      | #10 | Serial input pin. Write value in extender. (SPI MOSI)                  |
| `LATCH`          | #11 | Write extender selector.                                               |
| `LCD_WRITE`      | #13 | Display write pin. Refer to ["Using display"](#using-display) section. |

### Epson_PNL_CE02 Class

#### Constructor

| Parameter        | Description                                                            |
| ---------------- | ---------------------------------------------------------------------- |
| `pPinout`        | Reference to [`Epson_PNL_CE02_Pinout`](#epson_pnl_ce02_pinout-struct). |

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

| Button      | Mask                                                                                                            |
| ----------- | --------------------------------------------------------------------------------------------------------------- |
| **`HOME`**  | `00000001`                                                                                                      |
| **`STOP`**  | `00000010`                                                                                                      |
| **`DOWN`**  | `00000100`                                                                                                      |
| **`START`** | `00001000`                                                                                                      |
| **`LEFT`**  | `00010000`                                                                                                      |
| **`UP`**    | `00100000`                                                                                                      |
| **`OK`**    | `01000000`                                                                                                      |
| **`RIGHT`** | `10000000`                                                                                                      |
| ~~`POWER`~~ | The power button has a dedicated pin (refer to [`Epson_PNL_CE02_Pinout Struct`](#epson_pnl_ce02_pinout-struct)) |

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


## Like this project? ❤️

⭐ [Star this project](https://github.com/XavierBrassoud/Arduino_Epson_PNL_CE02/star) to provide better visibility to the community!

💻 Support this project by purchasing [affiliated hardwares](https://amzn.to/3y5obqz)

☕ [Buy me a coffee](https://paypal.me/XavierBrassoud)

Thank you!
