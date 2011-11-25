# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005
#
############################################################
# You should not have to change anything here!
############################################################


# version (bases on MegaBOOT 050815)
BUILD      = 111124

# program name should not be changed...
PROGRAM    = ATmegaBOOT

ifndef PRODUCT
#echo "warning: PRODUCT undefined! Use PRODUCT=DEFAULT"
PRODUCT    = DEFAULT
endif

include projects/product_$(PRODUCT).mak
include projects/isp_$(PRODUCT).mak

# --MAP-- map the MCU_TARGET to an avr-dude target
ifeq ($(MCU_TARGET),atmega8)
    ISP_TARGET=m8
else ifeq ($(MCU_TARGET),atmega168)
    ISP_TARGET=m168
else ifeq ($(MCU_TARGET),atmega32)
    ISP_TARGET=m32
else ifeq ($(MCU_TARGET),atmega324p)
    ISP_TARGET=m324p
else ifeq ($(MCU_TARGET),atmega644)
    ISP_TARGET=m644
else ifeq ($(MCU_TARGET),atmega644p)
    ISP_TARGET=m644p
else ifeq ($(MCU_TARGET),atmega128)
    ISP_TARGET=m128
else ifeq ($(MCU_TARGET),atmega2560)
    ISP_TARGET=m2560
endif

LDSECTION  = --section-start=.text=$(BOOT_START)
ISPFUSES   = avrdude -c $(ISPTOOL) -p $(ISP_TARGET) -P $(ISPPORT) $(ISPSPEED) -u -U efuse:w:$(ISP_EFUSE):m -U hfuse:w:$(ISP_HFUSE):m -U lfuse:w:$(ISP_LFUSE):m
ISPFLASH   = avrdude -c $(ISPTOOL) -p $(ISP_TARGET) -P $(ISPPORT) $(ISPSPEED) -V -U flash:w:$(PROGRAM)_$(PRODUCT)_$(BUILD).hex


OBJ        = $(PROGRAM).o
OPTIMIZE   = -Os

DEFS   = -D$(PRODUCT)
ifdef USE_CONFIG_H
DEFS  += "-DCONFIG_H=1"
endif
ifdef WELCOME_MSG
DEFS  += '-DWELCOME_MSG=$(WELCOME_MSG)'
endif
DEFS  += "-DUSE_MONITOR=$(USE_MONITOR)"
ifdef BAUDRATE
DEFS  += "-DBAUD_RATE=$(BAUDRATE)"
endif
DEFS  += "-DLED_DDR=$(LED_DDR)"
DEFS  += "-DLED_PORT=$(LED_PORT)"
DEFS  += "-DLED_PIN=$(LED_PIN)"
DEFS  += "-DLED=$(LED)"
ifdef BL_PIN
DEFS  += "-DBL_DDR=$(BL_DDR)"
DEFS  += "-DBL_PORT=$(BL_PORT)"
DEFS  += "-DBL_PIN=$(BL_PIN)"
DEFS  += "-DBL=$(BL)"
endif

LIBS       =

CC         = avr-gcc


# Override is only needed by avr-lib build system.

override CFLAGS  = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -DF_CPU=$(F_CPU)UL $(DEFS)
override LDFLAGS = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)

OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE	= avr-size


all: checking $(PROGRAM).elf lst text

checking:
ifndef ISP_EFUSE
	@echo "error: ISP_EFUSE undefined"
	@exit 1
endif
ifndef ISP_HFUSE
	@echo "error: ISP_HFUSE undefined"
	@exit 1
endif
ifndef ISP_LFUSE
	@echo "error: ISP_LFUSE undefined"
	@exit 1
endif
ifndef ISP_TARGET
	@echo "error: ISP_TARGET undefined"
	@exit 1
endif
ifndef MCU_TARGET
	@echo "error: MCU_TARGET undefined"
	@exit 1
endif
ifndef ISP_TARGET
	@echo "error: ISP_TARGET undefined. Add it to this Makefile (see --MAP--)"
	@exit 1
endif
	@echo "checking done..."

isp: $(PROGRAM).hex
	$(ISPFUSES)
	$(ISPFLASH)

$(PROGRAM).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

ifdef USE_CONFIG_H
$(PROGRAM).o:	$(PROGRAM).c config.h

config.h:	projects/config_$(PRODUCT).h
		cp projects/config_$(PRODUCT).h config.h
endif

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep config.h
	rm -rf $(PROGRAM).hex $(PROGRAM).srec $(PROGRAM).bin

lst:  $(PROGRAM).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec size

hex:  $(PROGRAM).hex
bin:  $(PROGRAM).bin
srec: $(PROGRAM).srec


%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(BUILD).hex

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(BUILD).srec

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(BUILD).bin

size:
	$(AVRSIZE) --target ihex $(PROGRAM).hex
