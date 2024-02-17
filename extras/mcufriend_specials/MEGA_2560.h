/**
 * @file MEGA_2560.h
 * @author Xavier BRASSOUD (contact@xavierbrassoud.fr)
 * @brief Based on MCUFRIEND_kbv special "USE_MEGA_8BIT_PORTC_SHIELD".
 * Copy-paste this new special inside MCUFRIEND_kbv/utility/mcufriend_special.h
 *
 * Need explicit instantiation of Epson_PNL_CE02 as it's used here as extern member:
 * Epson_PNL_CE02 controlPanel(...);
 *
 * See examples/display, or examples/full for complete usage.
 *
 * This special is adapted for a MEGA2560 with the following pin mapping:
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
 * @version 0.1
 * @date 2024-02-13
 *
 * @copyright MIT license
 *
 */

// Paste to the top of mcufriend_special.h
#define USE_EPSON_PNL_CE02 // 6.39sec

// Paste to the SPECIAL definitions of mcufriend_special.h, somewhere between #if and #else
#elif defined(__AVR_ATmega2560__) && defined(USE_EPSON_PNL_CE02)
// LCD pins          |D7  |D6  |D5  |D4  |D3  |D2  |D1  |D0  |  |RD  |WR  |RST |RS       |CS         |
// EPSON_PNL_CE02    |     Parallel multiplexer 9 & 10       |  |VCC |WR  |RST | Register 9, 10 & 11 |
// ATmega2560        |      PG2 (SCK) | PG1 (SER IN)         |  |-   |PL0 |PL2 |PB1   |PB2   |PL1    |
// MEGA2560pin       |               51 | 52                 |  |-   |49  |47  | 52   |51    |48     |

#include <Arduino.h>
#include <Epson_PNL_CE02.h>
extern Epson_PNL_CE02 controlPanel;

#warning USE_MEGA_8BIT_SHIELD for EPSON_PNL_CE02

#define WR_PORT PORTL
#define WR_PIN 0 // D49 CTE
#define RESET_PORT PORTL
#define RESET_PIN 2 // D25 CTE

#define VIRTUAL_PORT _MMIO_BYTE(0XFF) // Set Virtual Port for PIN_ methods
#define CD_PORT VIRTUAL_PORT
#define CD_PIN LCD_DC // D4 VHC595
#define CS_PORT VIRTUAL_PORT
#define CS_PIN LCD_CS // D3 VHC595

#define UNUSED_PORT _MMIO_BYTE(0XFE) // Disable READ methods as the control panel wired the display in write-only mode
#define RD_PORT UNUSED_PORT
#define RD_PIN -1

#define write_8(x) ({ controlPanel.displayWrite(x); })

#define read_8() (PINC)
#define setWriteDir()                                                                                                  \
    {                                                                                                                  \
        DDRC = 0xFF;                                                                                                   \
    }
#define setReadDir()                                                                                                   \
    {                                                                                                                  \
        DDRC = 0x00;                                                                                                   \
    }
#define write8(x)                                                                                                      \
    {                                                                                                                  \
        write_8(x);                                                                                                    \
        WR_ACTIVE;                                                                                                     \
        WR_STROBE;                                                                                                     \
    }
#define write16(x)                                                                                                     \
    {                                                                                                                  \
        uint8_t h = (x) >> 8, l = x;                                                                                   \
        write8(h);                                                                                                     \
        write8(l);                                                                                                     \
    }
#define READ_8(dst)                                                                                                    \
    {                                                                                                                  \
        RD_STROBE;                                                                                                     \
        dst = read_8();                                                                                                \
        RD_IDLE;                                                                                                       \
    }
#define READ_16(dst)                                                                                                   \
    {                                                                                                                  \
        RD_STROBE;                                                                                                     \
        dst = read_8();                                                                                                \
        RD_IDLE;                                                                                                       \
        RD_STROBE;                                                                                                     \
        dst = (dst << 8) | read_8();                                                                                   \
        RD_IDLE;                                                                                                       \
    }

#define PIN_LOW(p, b)                                                                                                  \
    ({                                                                                                                 \
        if (&p != &VIRTUAL_PORT)                                                                                       \
        {                                                                                                              \
            (p) &= ~(1 << (b));                                                                                        \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            controlPanel.extenderWrite((ExtenderPin)b, LOW);                                                           \
        }                                                                                                              \
    })

#define PIN_HIGH(p, b)                                                                                                 \
    ({                                                                                                                 \
        if (&p != &VIRTUAL_PORT)                                                                                       \
        {                                                                                                              \
            (p) |= (1 << (b));                                                                                         \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            controlPanel.extenderWrite((ExtenderPin)b, HIGH);                                                          \
        }                                                                                                              \
    })

#define PIN_OUTPUT(p, b)                                                                                               \
    ({                                                                                                                 \
        if (&p != &VIRTUAL_PORT)                                                                                       \
        {                                                                                                              \
            *(&p - 1) |= (1 << (b));                                                                                   \
        }                                                                                                              \
    })
