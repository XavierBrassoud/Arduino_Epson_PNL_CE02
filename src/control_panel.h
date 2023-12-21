#include <Arduino.h>
#include <stdint.h>

enum Key
{
    RIGHT = 0b00000001, // 0b11111110
    OK = 0b00000010,    // 0b11111101
    UP = 0b00000100,    // 0b11111011
    LEFT = 0b00001000,  // 0b11110111
    START = 0b00010000, // 0b11101111
    DOWN = 0b00100000,  // 0b11011111
    STOP = 0b01000000,  // 0b10111111
    HOME = 0b10000000,  // 0b01111111
};

const char *keyNames[8] = {
    "Right",
    "Ok",
    "Up",
    "Left",
    "Start",
    "Down",
    "Stop",
    "Home",
};

const char *keyName(Key key)
{
    const double bitPos = log(key) / log(2);
    return keyNames[(int)bitPos];
}

const bool isKeyPressed(byte keys, Key key)
{
    return (keys & key) != 0; // Check if key pressed 0000{key}000, key>0 if set
}

class ControlPanel
{

    int _serOutPin;
    int _clockPin;
    int _shiftLoadPin;

    unsigned long _debounceDelay;
    unsigned long _longPressDelay;

    byte _pressedKeys = 0b00000000;
    byte _longPressedKeys = 0b00000000;
    byte _shortPressedKeys = 0b00000000;

    // timers register when a key is pressed
    unsigned long _keysTimers[8];

public:
    ControlPanel(int oePin, int serOutPin, int clockPin, int serInPin, int shiftLoadPin, unsigned long debounceDelay = 50, unsigned long longPressDelay = 2000)
    {
        _serOutPin = serOutPin;
        _clockPin = clockPin;
        _shiftLoadPin = shiftLoadPin;

        _debounceDelay = debounceDelay;
        _longPressDelay = longPressDelay;

        pinMode(_shiftLoadPin, OUTPUT);
        pinMode(_clockPin, OUTPUT);
        pinMode(_serOutPin, INPUT);
    }

    /**
     * @brief Read current pressed keys in binary format of 1 byte (0: no pressed, 1: pressed).
     *
     * @return byte Keys pressed.
     *
     * @example
     * ``` c++
     * const cp = ControlPanel(...);
     * cp.read();
     * switch(cp.readKeys()) {
     *  case Key.HOME:
     *      Serial.println("Key HOME pressed.");
     *      break;
     *  case Key.HOME | Key.OK:
     *      Serial.println("Key HOME and key OK pressed.");
     *      break;
     * }
     * ```
     */
    byte readKeys()
    {
        return _pressedKeys;
    }

    /**
     * @brief Read last pressed keys in binary format of 1 byte (0: no pressed, 1: pressed).
     *
     * @return byte Keys pressed.
     */
    byte readPressedKeys()
    {
        return _shortPressedKeys | _longPressedKeys;
    }

    /**
     * @brief Read short pressed keys in binary format of 1 byte (0: no pressed, 1: pressed).
     *
     * @return byte
     */
    byte readShortPressedKeys()
    {
        return _shortPressedKeys;
    }

    /**
     * @brief Read long pressed keys in binary format of 1 byte (0: no pressed, 1: pressed).
     *
     * @return byte
     */
    byte readLongPressedKeys()
    {
        return _longPressedKeys;
    }

    /**
     * @brief Read inputs loop.
     *
     */
    void read()
    {
        int value;
        byte dataIn = 0b00000000;

        digitalWrite(_shiftLoadPin, LOW);  // enables parallel inputs
        digitalWrite(_clockPin, LOW);      // start clock pin low
        digitalWrite(_clockPin, HIGH);     // set clock pin high, data loaded into SR
        digitalWrite(_shiftLoadPin, HIGH); // disable parallel inputs and enable serial output

        for (int i = 0; i < 8; i++)
        {
            value = !digitalRead(_serOutPin); // reads data from SR serial data out pin (invert cause output is HIGH)
            if (value)
            {
                int a = (1 << i);    // shifts bit to its proper place in sequence
                dataIn = dataIn | a; // combines data from shifted bits to form a single 8-bit number
            }
            digitalWrite(_clockPin, LOW);  // after each bit is logged
            digitalWrite(_clockPin, HIGH); // pulses clock to get next bit
        }

        if (_pressedKeys != dataIn)
        {
            _pressedKeys = dataIn;
        }
        keysDispatcher(_pressedKeys);
    }

private:
    // Dispatch keys on short/long pressed registers.
    void keysDispatcher(byte keys)
    {
        byte longPress = 0b00000000;
        byte shortPress = 0b00000000;

        for (int i = 0; i < 8; i++)
        {
            Key key = Key(0b00000000 | (1 << i));
            bool wasPressed = _keysTimers[i] != 0;
            bool isPressed = isKeyPressed(keys, key);

            // Button pressed 1st time
            if (!wasPressed && isPressed)
            {
                _keysTimers[i] = millis();
            }
            // Button released, let's calculate his press delay in order to dispatch
            else if (wasPressed && !isPressed)
            {
                const unsigned long duration = millis() - _keysTimers[i];
                if (duration > _debounceDelay)
                {
                    if (duration < _longPressDelay)
                    {
                        Serial.print("Short press: ");
                        Serial.println(duration);

                        shortPress = shortPress | 1 << i;
                    }
                    else
                    {
                        Serial.print("Long press: ");
                        Serial.println(duration);

                        longPress = longPress | (1 << i);
                    }
                }
                _keysTimers[i] = 0;
            }
        }
        _shortPressedKeys = shortPress;
        _longPressedKeys = longPress;
    }
};
