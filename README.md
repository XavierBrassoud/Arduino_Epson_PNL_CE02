# Arduino - EPSON XP 520/530/540 Control Panel (PNL CE02)

Library to repurposing the control panel (PNL CE02) of EPSON XP 520/530/540 printers from an Arduino.

## Installation

TBD

## PIN

The FFC from the CPU to the control board has this pinout:

| Pin | Purpose                                   |
|-----|-------------------------------------------|
| 1   | 3-STATE Output Enable Input (OE)          |
| 2   | Serial Data Output (SER OUT)              |
| 3   | GND                                       |
| 4   | on/off btn                                |
| 5   | 3.3V supply                               |
| 6   | LCD UNKNOWN                               |
| 7   | LCD VIN UNKNOWN                           |
| 8   | GND                                       |
| 9   | Shift Register Clock Input (SCK)          |
| 10  | Serial Data Input (SER IN)                |
| 11  | Storage Register Clock Input (RCK)        |
| 12  | GND                                       |
| 13  | Unknown                                   |
| 14  | GND                                       |


## Let's play!

``` c++
Epson_PNL_CE02 controlPanel(...);
...
switch (controlPanel.readButtons())
{
    case OK:
        Serial.println("Key OK pressed !");
        break;
    case OK | LEFT:
        Serial.println("Key OK and key LEFT pressed !");
        break;
}
```

See more usages in [`examples`](./examples/) folder.


## Credits

Thanks [@phooky](https://github.com/phooky) for your [inspiriting guide](https://www.nycresistor.com/2022/01/18/repurposing-control-panel/)!

Examples uses:
 * [@mathertel](https://github.com/mathertel)'s [OneButton](https://github.com/mathertel/OneButton) library (BSD license).

EPSON for the control panel 😉
