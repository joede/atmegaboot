## product_CRUMB128.mak -- ATmegaBOOT                 -*- Makefile -*-
##
## Configuration of projects supported by ATmegaBOOT

PRODUCT    = CRUMB8
F_CPU = 14745600
MCU_TARGET = atmega8
ISP_TARGET = m8
BOOT_START = 0x1800
ISP_HFUSE  = 0xc8
ISP_LFUSE  = 0xdf

USE_MONITOR=0

## Onboard LED is connected to pin PB2 (e.g. Crumb8, Crumb168)
LED_DDR=DDRB
LED_PORT=PORTB
LED_PIN=PINB
LED=PINB2

## Input pin to allow selection of the bootloader
USE_UART=0
BL_DDR=DDRD
BL_PORT=PORTD
BL_PIN=PIND
BL=PIND6

## --------- end of file ---------
