## product_SAVIMASTER.mak -- ATmegaBOOT             -*- Makefile -*-
##
## Project: SaVi.Master

## use a optional config.h header
USE_CONFIG_H = 1

## the oszillator frequency
F_CPU        = 11059200

## the target type for the GCC calls
MCU_TARGET   = atmega128

## The starting address of the bootloader. Depends on the fuses!
## starts at 0xF000 words => 0x1E000 bytes
BOOT_START   = 0x1E000

## The fuses. For AVRs without efuse, comment out this line
#
# we don't call the bootloader at powerup!
#
ISP_EFUSE    = 0xFF
ISP_HFUSE    = 0xC9
ISP_LFUSE    = 0x3F

## Set to 1 to enable the monitor
USE_MONITOR  = 1

## The default baudrate. Uncomment this line to change the default 115200bps!
BAUDRATE     = 38400

## Onboard LED is connected to pin PB3
## Define the port-, the DDR-, the pin register and the pin number of the output,
## where the "Bootloader LED" is connected too.
LED_DDR      = DDRB
LED_PORT     = PORTB
LED_PIN      = PINB
LED          = PINB3

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
