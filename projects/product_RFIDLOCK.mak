## product_RFIDLOCK.mak -- ATmegaBOOT             -*- Makefile -*-
##
## Project: rfid-lock.avr

## use a optional config.h header
USE_CONFIG_H = 1

## the oszillator frequency
F_CPU        = 8000000

## the target type for the GCC calls
MCU_TARGET   = atmega324p

## The starting address of the bootloader. Depends on the fuses!
## starts at 0x3800 words => 0x7000 bytes
BOOT_START   = 0x7000

## The fuses. For AVRs without efuse, comment out this line
#
# we don't call the bootloader at powerup!
#
ISP_EFUSE    = 0xFd
ISP_HFUSE    = 0xD9
ISP_LFUSE    = 0xE0

## Set to 1 to enable the monitor
USE_MONITOR  = 0

## The default baudrate. Uncomment this line to change the default 115200bps!
BAUDRATE     = 38400

## Onboard LED is connected to pin PD4
## Define the port-, the DDR-, the pin register and the pin number of the output,
## where the "Bootloader LED" is connected too.
LED_DDR      = DDRD
LED_PORT     = PORTD
LED_PIN      = PIND
LED          = PIND4

## Input pin to allow selection of the bootloader (Mega128)
## Define the port-, the DDR-, the pin register and the pin number of the
## input, which must be pulled low to enter the bootloader.
#
# no BL pin here!
#
#BL_DDR       = DDRD
#BL_PORT      = PORTD
#BL_PIN       = PIND
#BL           = PIND5

## --------- end of file ---------
