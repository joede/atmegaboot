## product_CRUMB168.mak -- ATmegaBOOT            -*- Makefile -*-
##
## Configuration of projects supported by ATmegaBOOT

PRODUCT    = CRUMB168
F_CPU      = 20000000
MCU_TARGET = atmega168
ISP_TARGET = m168
BOOT_START = 0x3800
ISP_EFUSE  = 0xf8
ISP_HFUSE  = 0xd7
ISP_LFUSE  = 0xaf

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
