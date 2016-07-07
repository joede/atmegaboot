# Project: ATmegaBOOT

This is an adaption of the bootloader code from http://chip45.com/. I've
changed the code to make it easier to configure and to match my requirements.

This code is was a first try and --as expected-- it wasn't the last one. ;-)
ATmegaBOOT is a *stk500v1* based bootloader. This means, it is (and will ever be)
**limited to 128kB flash size**! As long as the size of the firmware is below
this limit, the port to *ATmega2560* or *ATmega2561* can be used!

Nevertheless, as a result of this limitation  I've started using a *stk500v2* 
based bootloader to allow code sizes with more than 128kB.
See [stk500v2-bootloader](https://github.com/joede/stk500v2-bootloader).

To add support for a new product, copy the `product_TEMPLATE.mak` to a new file
and replace "TEMPLATE" with a unique shortcut for the products name. Change
the file to fit your need. If necessary, copy `config_TEMPLATE.h` and
`isp_TEMPLATE.mak` and replace "TEMPLATE" with the same shortcut.

**Attention:** be very careful while editing the fuses in `product_*.mak`!
It is possible to **damage your MCU**!

To build the bootloader, call `make PRODUCT=`TEMPLATE and replace "TEMPLATE"
with the shortcut again. As a result, you will get three binaries (bin,
Intel-Hex and Motorola-SRecord).

So the first steps for a product "FOO" could look like this:

~~~~
$ cp product_TEMPLATE.mak projects/product_FOO.mak
$ editor product_FOO.mak
$ cp isp_TEMPLATE.mak projects/isp_FOO.mak
$ editor isp_FOO.mak
$ cp config_TEMPLATE.h projects/config_FOO.h
$ editor config_FOO.h
$ make PRODUCT=FOO
$ ls -1 ATmegaBOOT_FOO*
$ ATmegaBOOT_FOO_V1_0.bin
$ ATmegaBOOT_FOO_V1_0.hex
$ ATmegaBOOT_FOO_V1_0.srec
~~~~

**Note:** to configure the ISP settings, just edit `isp_FOO.mak`!

To upload a firmware after the bootloader has been started, just enter a command
line like this:

~~~~
$ avrdude -p atmega128 -P /dev/ttyUSB0 -c stk500v1 -b 38400 -U flash:w:foo-1.1.2.hex
~~~~

## Enter the bootloader

In most cases, the bootloader pin is used to enter the bootloader. This pin
must be pressed while power on the MCU. If there is no such pin available, the
*forced-mode* can be used.

The bootloader can be configured (USE_FORCED_BOOTLOAD_ENTER) to wait 3 seconds
for a sequence of 5+ consecutive '*' at the UART. If this '*' sequence is
received, the bootloader starts sending '*' and wait for an empty UART buffer.
If no more data is fetched, the regular bootloader is entered.

In both cases, the fuses must be configured to enter the bootloader after
power-up!


## Enter the bootloader from within the application

It is possible to call the bootloader from with your application. To
do this, use the address defined inside the standard Makefile (it should be
there) as byte address. Inside the C code, use this define to initialise a
function pointer.

~~~~
#ifdef BOOTSTART
void (*__bootloader)(void) = (void*)BOOTSTART;
#else
#  error "error: BOOTSTART isn't defined!"
#endif
~~~~

**Note:** This is currently only tested for MCUs with an bootloader below 64kB!


## Leaving the bootloader

The bootloader is still active after avrdude has done it's job! You must
restart your device to enter the application! This can be changed with the
`WANT_START_APP` compiler flag in `config_*.h`. Enable this feature only if the
application can run outside it's intended environment.

Have fun.
