## product_CRUMB128.mak -- ATmegaBOOT                   -*- Makefile -*-
##
## Configuration of projects supported by ATmegaBOOT

PRODUCT    = CRUMB128
WELCOME_MSG= "ATmegaBOOT / Crumb128 - (C) J.P.Kyle, E.Lins - 050815\n\r"
F_CPU      = 14745600
MCU_TARGET = atmega128
ISP_TARGET = m128
BOOT_START = 0x1E00
ISP_EFUSE  = 0xff
ISP_HFUSE  = 0xc8
ISP_LFUSE  = 0xdf

USE_MONITOR=1

## Onboard LED is connected to pin PB7 (e.g. Crumb128, PROBOmega128, Savvy128)
LED_DDR=DDRB
LED_PORT=PORTB
LED_PIN=PINB
LED=PINB7

## Input pin to allow selection of the bootloader (Mega128)
USE_UART=0
BL_DDR=DDRF
BL_PORT=PORTF
BL_PIN =PINF
BL=PINF7

## --------- end of file ---------
