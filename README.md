# Project: ATmegaBOOT

This is an adaption of the bootloader code from http://chip45.com/. I've
changed the code to make it easier to configure and to match my requirements.

This code is a first try and I don't know if it is the last one. ;-)

To add support for a new product, copy the `product_TEMPLATE.mak` to a new file
and replace "TEMPLATE" with a unique shortcut for the products name. Change
the file to fit your need. If necessary, copy `config_TEMPLATE.h` and
`isp_TEMPLATE.mak` and replace "TEMPLATE" with the same shortcut.

**Attention:** be very carefull while editing the fuses in `product_*.mak`!
It is possible to **damage your MCU**!

To build the bootloader, call `make PRODUCT=`TEMPLATE and replace "TEMPLATE"
with the shortcut again. As a result, you will get three binaries (bin,
Intel-Hex and Motorola-SRecord).

So the first steps for a product "FOO" could look like this:

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

**Note:** to configure the ISP settings, just edit `isp_FOO.mak`!


Have fun.