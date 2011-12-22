/* ATmegaBOOT -- Serial Bootloader for Atmel megaAVR Controllers
 * -----------------------------------------------------------------------------
 *
 * Release: V1.0
 * date : 2.13.2011
 *
 * Tested with: ATmega8, ATmega128, ATmega324P
 * should work with other mega's, see code for details
 *
 * -----------------------------------------------------------------------------
 * V1.0  first release without upstream. Modify define BL_RELEASE to change the
 *       new version string and don't forget the Makefile!
 *
 * -----------------------------------------------------------------------------
 * Changes made by Joerg Desch <jd@voelker-web.de> are documented in the new
 * History file!
 *
 * Monitor and debug functions were added to the original
 * code by Dr. Erik Lins, chip45.com. (See below)
 *
 * Thanks to Karl Pitrich for fixing a bootloader pin
 * problem and more informative LED blinking!
 *
 * For the latest version see:
 * http://www.chip45.com/
 *
 * -----------------------------------------------------------------------------
 * based on stk500boot.c
 * Copyright (c) 2003, Jason P. Kyle
 * All rights reserved.
 * see avr1.org for original file and information
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with this program; if not, write
 * to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Licence can be viewed at
 * http://www.fsf.org/licenses/gpl.txt
 *
 * Target = Atmel AVR m128,m64,m32,m16,m8,m162,m163,m169,
 * m8515,m8535. ATmega161 has a very small boot block so
 * isn't supported.
 *
 * Tested with m128,m8,m163 - feel free to let me know
 * how/if it works for you.
 *
 * -----------------------------------------------------------------------------
 */


/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define BL_RELEASE "V1.0"

/* the current avr-libc eeprom functions do not support the ATmega168 */
/* own eeprom write/read functions are used instead */
#ifndef __AVR_ATmega168__
#  include <avr/eeprom.h>
#endif

#ifdef CONFIG_H
#  include "config.h"
#endif

#if !defined F_CPU
#  error "Error: F_CPU must be defined in the product makefile!"
#endif


/* set the UART baud rate */
#ifndef BAUD_RATE
#  define BAUD_RATE   115200
#endif


/* SW_MAJOR and MINOR needs to be updated from time to time to avoid warning
 * message from AVR Studio.
 * IMPORTANT: never allow AVR Studio to do an update!!!!
 */
#define HW_VER	 0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x10


/* monitor functions will only be compiled when using ATmega128, due to
 * bootblock size constraints
 */
#if USE_MONITOR
#  define MONITOR
#endif


/* define various device id's and there page sizes.
 * The manufacturer byte is always the same...
 * Yep, Atmel is the only manufacturer of AVR micros.  Single source :(
 */
#define SIG1	0x1E

#if defined __AVR_ATmega128__
  #define SIG2	0x97
  #define SIG3	0x02
  #define PAGE_SIZE	0x80U	//128 words

#elif defined __AVR_ATmega64__
  #define SIG2	0x96
  #define SIG3	0x02
  #define PAGE_SIZE	0x80U	//128 words

#elif defined __AVR_ATmega644P__
  #define SIG2	0x96
  #define SIG3	0x0A
  #define PAGE_SIZE	0x80U   //128 words

#elif defined __AVR_ATmega644__
  #define SIG2	0x96
  #define SIG3	0x09
  #define PAGE_SIZE	0x80U   //128 words

#elif defined __AVR_ATmega32__
  #define SIG2	0x95
  #define SIG3	0x02
  #define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega324P__
  #define SIG2	0x95
  #define SIG3	0x08
  #define PAGE_SIZE	0x40U   //64 words

#elif defined __AVR_ATmega16__
  #define SIG2	0x94
  #define SIG3	0x03
  #define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega8__
  #define SIG2	0x93
  #define SIG3	0x07
  #define PAGE_SIZE	0x20U	//32 words

#elif defined __AVR_ATmega88__
  #define SIG2	0x93
  #define SIG3	0x0a
  #define PAGE_SIZE	0x20U	//32 words

#elif defined __AVR_ATmega168__
  #define SIG2	0x94
  #define SIG3	0x06
  #define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega162__
  #define SIG2	0x94
  #define SIG3	0x04
  #define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega163__
  #define SIG2	0x94
  #define SIG3	0x02
  #define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega169__
  #define SIG2	0x94
  #define SIG3	0x05
  #define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega8515__
  #define SIG2	0x93
  #define SIG3	0x06
  #define PAGE_SIZE	0x20U	//32 words

#elif defined __AVR_ATmega8535__
  #define SIG2	0x93
  #define SIG3	0x08
  #define PAGE_SIZE	0x20U	//32 words

#else
  #error "error: unknown target!"
#endif


/* function prototypes */
void putsP (PGM_P s);
void putch(char);
char getch(void);
void getNch(uint8_t);
void byte_response(uint8_t);
void nothing_response(void);
char gethex(void);
void puthex(char);
void flash_led(uint8_t);
#ifdef WANT_WAIT_BL
void wait_bl_pin(void);
#endif

/* some variables */
union address_union {
    uint16_t word;
    uint8_t  byte[2];
} address;

union length_union {
    uint16_t word;
    uint8_t  byte[2];
} length;

struct flags_struct {
    unsigned eeprom : 1;
    unsigned rampz  : 1;
} flags;

uint8_t buff[256];
uint8_t address_high;

// TODO: not used!?
// uint8_t pagesz=0x80;

uint8_t i;
uint8_t bootuart = 0;

/* Entry point of the application. Another way may be the
 * autoreset via watchdog (sneaky!) BBR/LF 9/13/2008
 * WDTCSR = _BV(WDE);
 * while (1); // 16 ms
 */
void (*app_start)(void) = 0x0000;


/* main program starts here */
int main(void)
{
    uint8_t ch,ch2;
    uint16_t w;

    asm volatile("nop\n\t");
#ifdef HANDLE_WDT_RESET
    if ( MCUSR & _BV(WDRF) )
	app_start();
#endif
    wdt_disable();
    cli();

    /* optionally setup the CPU ports
     */
#ifdef INIT_PORT_A
    PORTA = VAL_PORT_A;  DDRA = DIR_PORT_A;
#endif
#ifdef INIT_PORT_B
    PORTB = VAL_PORT_B;  DDRB = DIR_PORT_B;
#endif
#ifdef INIT_PORT_C
    PORTC = VAL_PORT_C;  DDRC = DIR_PORT_C;
#endif
#ifdef INIT_PORT_D
    PORTD = VAL_PORT_D;  DDRD = DIR_PORT_D;
#endif
#ifdef INIT_PORT_E
    PORTE = VAL_PORT_E;  DDRE = DIR_PORT_E;
#endif
#ifdef INIT_PORT_F
    PORTF = VAL_PORT_F;  DDRF = DIR_PORT_F;
#endif
#ifdef INIT_PORT_G
    PORTG = VAL_PORT_G;  DDRG = DIR_PORT_G;
#endif
#ifdef INIT_PORT_H
    PORTH = VAL_PORT_H;  DDRH = DIR_PORT_H;
#endif


    /* set pin direction for bootloader pin and enable pullup
     */
#ifdef BL_PIN
    BL_DDR &= ~_BV(BL);
    BL_PORT |= _BV(BL);
#endif

#ifdef RS485_TXON
    RS485_DDR |= _BV(RS485_TXON);
    RS485_PORT &= ~_BV(RS485_TXON);	     /* disable RS485 transmitter */
#endif

    /* check if flash is programmed already, if not start bootloader anyway */
#ifdef BL_PIN
    if ( pgm_read_byte_near(0x0000) != 0xFF )
    {
	/* check if bootloader pin is set low */
	if(bit_is_set(BL_PIN, BL)) {
          app_start();
        }
    }
#endif

    /* initialize UART(s) depending on CPU defined */
#ifdef USE_UART
    bootuart = USE_UART;
#else
    bootuart = 0;
#endif

    w = (uint16_t)((F_CPU / ((BAUD_RATE)<<3) + 1UL) / 2UL) - 1;

#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega324P__)
    if ( bootuart == 0 ) {
	UBRR0L = (uint8_t)(w&0x00FF);
	UBRR0H = (uint8_t)(w>>8);
	UCSR0A = 0x00;
	UCSR0C = 0x06;
	UCSR0B = _BV(TXEN0)|_BV(RXEN0);
    }
    else if ( bootuart == 1 ) {
	UBRR1L = (uint8_t)(w&0x00FF);
	UBRR1H = (uint8_t)(w>>8);
	UCSR1A = 0x00;
	UCSR1C = 0x06;
	UCSR1B = _BV(TXEN1)|_BV(RXEN1);
    }
#elif defined __AVR_ATmega163__
    UBRR = (uint8_t)(w&0x00FF);
    UBRRHI = (uint8_t)(w>>8);
    UCSRA = 0x00;
    UCSRB = _BV(TXEN)|_BV(RXEN);
#else
    /* m8,m16,m32,m169,m8515,m8535 */
    UBRRL = (uint8_t)(w&0x00FF);
    UBRRH = (uint8_t)(w>>8);
    UCSRA = 0x00;
    UCSRC = 0x06;
    UCSRB = _BV(TXEN)|_BV(RXEN);
#endif

    /* set LED pin as output */
    LED_DDR |= _BV(LED);
    LED_PORT |= _BV(LED);

    /* flash onboard LED to signal entering of bootloader */
    flash_led(3);

    putch('\0');

#if 1
    // simple "hello" for debugging...
    putch('U');
    putch('U');
    putch('U');
    putch('U');
    putch('U');
#endif

    /* forever loop */
    for (;;) {

	/* get character from UART */
	ch = getch();

	/* A bunch of if...else if... gives smaller code than switch...case ! */

	/* Hello is anyone home ? */
	if(ch=='0') {
	    nothing_response();
	}


	/* Request programmer ID */
	/* Not using PROGMEM string due to boot block in m128 being beyond 64kB boundry  */
	/* Would need to selectively manipulate RAMPZ, and it's only 9 characters anyway so who cares.  */
	else if(ch=='1') {
	    if (getch() == ' ') {
		putch(0x14);
		//putsP(PSTR("AVR ISP"))
		putch('A');
		putch('V');
		putch('R');
		putch(' ');
		putch('I');
		putch('S');
		putch('P');
		putch(0x10);
	    }
	}


	/* AVR ISP/STK500 board commands  DON'T CARE so default nothing_response */
	else if(ch=='@') {
	    ch2 = getch();
	    if (ch2>0x85) getch();
	    nothing_response();
	}


	/* AVR ISP/STK500 board requests */
	else if(ch=='A') {
	    ch2 = getch();
	    if(ch2==0x80) byte_response(HW_VER);		// Hardware version
	    else if(ch2==0x81) byte_response(SW_MAJOR);	// Software major version
	    else if(ch2==0x82) byte_response(SW_MINOR);	// Software minor version
	    else if(ch2==0x98) byte_response(0x03);		// Unknown but seems to be required by avr studio 3.56
	    else byte_response(0x00);				// Covers various unnecessary responses we don't care about
	}


	/* Device Parameters  DON'T CARE, DEVICE IS FIXED  */
	else if(ch=='B') {
	    getNch(20);
	    nothing_response();
	}


	/* Parallel programming stuff  DON'T CARE  */
	else if(ch=='E') {
	    getNch(5);
	    nothing_response();
	}


	/* Enter programming mode  */
	else if(ch=='P') {
	    nothing_response();
	}


	/* Leave programming mode  */
	else if(ch=='Q') {
	    nothing_response();
#ifdef WANT_WAIT_BL
	    wait_bl_pin();
#endif
	    flash_led(4);
#ifdef WANT_START_APP
	    app_start();
#endif
	}


	/* Erase device, don't care as we will erase one page at a time anyway.  */
	else if(ch=='R') {
	    nothing_response();
	}


	/* Set address, little endian. EEPROM in bytes, FLASH in words  */
	/* Perhaps extra address bytes may be added in future to support > 128kB FLASH.  */
	/* This might explain why little endian was used here, big endian used everywhere else.  */
	else if(ch=='U') {
	    address.byte[0] = getch();
	    address.byte[1] = getch();
	    nothing_response();
	}


	/* Universal SPI programming command, disabled.  Would be used for fuses and lock bits.  */
	else if(ch=='V') {
	    getNch(4);
	    byte_response(0x00);
	}


	/* Write memory, length is big endian and is in bytes  */
	else if(ch=='d') {
	    length.byte[1] = getch();
	    length.byte[0] = getch();
	    flags.eeprom = 0;
	    if (getch() == 'E') flags.eeprom = 1;
	    for (w=0;w<length.word;w++) {
		buff[w] = getch();	                        // Store data in buffer, can't keep up with serial data stream whilst programming pages
	    }
	    if (getch() == ' ') {
		if (flags.eeprom) {		                //Write to EEPROM one byte at a time
		    for(w=0;w<length.word;w++) {
#ifdef __AVR_ATmega168__
			while(EECR & (1<<EEPE));
			EEAR = (uint16_t)(void *)address.word;
			EEDR = buff[w];
			EECR |= (1<<EEMPE);
			EECR |= (1<<EEPE);
#else
			eeprom_write_byte((void *)address.word,buff[w]);
#endif
			address.word++;
		    }
		}
		else {					        //Write to FLASH one page at a time
		    if (address.byte[1]>127) address_high = 0x01;	//Only possible with m128, m256 will need 3rd address byte. FIXME
		    else address_high = 0x00;
#ifdef __AVR_ATmega128__
		    RAMPZ = address_high;
#endif
		    address.word = address.word << 1;	        //address * 2 -> byte location
		    /* if ((length.byte[0] & 0x01) == 0x01) length.word++;	//Even up an odd number of bytes */
		    if ((length.byte[0] & 0x01)) length.word++;	//Even up an odd number of bytes
		    cli();					//Disable interrupts, just to be sure
		    while(bit_is_set(EECR,EEWE));			//Wait for previous EEPROM writes to complete
		    asm volatile(
				 "clr	r17		\n\t"	//page_word_count
				 "lds	r30,address	\n\t"	//Address of FLASH location (in bytes)
				 "lds	r31,address+1	\n\t"
				 "ldi	r28,lo8(buff)	\n\t"	//Start of buffer array in RAM
				 "ldi	r29,hi8(buff)	\n\t"
				 "lds	r24,length	\n\t"	//Length of data to be written (in bytes)
				 "lds	r25,length+1	\n\t"
				 "length_loop:		\n\t"	//Main loop, repeat for number of words in block
				 "cpi	r17,0x00	\n\t"	//If page_word_count=0 then erase page
				 "brne	no_page_erase	\n\t"
				 "wait_spm1:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm1       \n\t"
				 "ldi	r16,0x03	\n\t"	//Erase page pointed to by Z
				 "sts	%0,r16		\n\t"
				 "spm			\n\t"
#ifdef __AVR_ATmega163__
				 ".word 0xFFFF		\n\t"
				 "nop			\n\t"
#endif
				 "wait_spm2:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm2       \n\t"

				 "ldi	r16,0x11	\n\t"	//Re-enable RWW section
				 "sts	%0,r16		\n\t"
				 "spm			\n\t"
#ifdef __AVR_ATmega163__
				 ".word 0xFFFF		\n\t"
				 "nop			\n\t"
#endif
				 "no_page_erase:		\n\t"
				 "ld	r0,Y+		\n\t"	//Write 2 bytes into page buffer
				 "ld	r1,Y+		\n\t"

				 "wait_spm3:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm3       \n\t"
				 "ldi	r16,0x01	\n\t"	//Load r0,r1 into FLASH page buffer
				 "sts	%0,r16		\n\t"
				 "spm			\n\t"

				 "inc	r17		\n\t"	//page_word_count++
				 "cpi r17,%1	        \n\t"
				 "brlo	same_page	\n\t"	//Still same page in FLASH
				 "write_page:		\n\t"
				 "clr	r17		\n\t"	//New page, write current one first
				 "wait_spm4:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm4       \n\t"
#ifdef __AVR_ATmega163__
				 "andi	r30,0x80	\n\t"	// m163 requires Z6:Z1 to be zero during page write
#endif
				 "ldi	r16,0x05	\n\t"	//Write page pointed to by Z
				 "sts	%0,r16		\n\t"
				 "spm			\n\t"
#ifdef __AVR_ATmega163__
				 ".word 0xFFFF		\n\t"
				 "nop			\n\t"
				 "ori	r30,0x7E	\n\t"	// recover Z6:Z1 state after page write (had to be zero during write)
#endif
				 "wait_spm5:		\n\t"
				 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
				 "andi	r16,1           \n\t"
				 "cpi	r16,1           \n\t"
				 "breq	wait_spm5       \n\t"
				 "ldi	r16,0x11	\n\t"	//Re-enable RWW section
				 "sts	%0,r16		\n\t"
				 "spm			\n\t"
#ifdef __AVR_ATmega163__
				 ".word 0xFFFF		\n\t"
				 "nop			\n\t"
#endif
				 "same_page:		\n\t"
				 "adiw	r30,2		\n\t"	//Next word in FLASH
				 "sbiw	r24,2		\n\t"	//length-2
				 "breq	final_write	\n\t"	//Finished
				 "rjmp	length_loop	\n\t"
				 "final_write:		\n\t"
				 "cpi	r17,0		\n\t"
				 "breq	block_done	\n\t"
				 "adiw	r24,2		\n\t"	//length+2, fool above check on length after short page write
				 "rjmp	write_page	\n\t"
				 "block_done:		\n\t"
				 "clr	__zero_reg__	\n\t"	//restore zero register
#if defined __AVR_ATmega168__  || __AVR_ATmega328P__ || __AVR_ATmega128__ || __AVR_ATmega1280__ || __AVR_ATmega1281__ || __AVR_ATmega1284P__ || __AVR_ATmega644P__ || __AVR_ATmega644__ || __AVR_ATmega324P__
				 : "=m" (SPMCSR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31"
#else
				 : "=m" (SPMCR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31"
#endif
				 );
		    /* Should really add a wait for RWW section to be enabled, don't actually need it since we never */
		    /* exit the bootloader without a power cycle anyhow */
		}
		putch(0x14);
		putch(0x10);
	    }
	}


        /* Read memory block mode, length is big endian.  */
        else if(ch=='t') {
	    length.byte[1] = getch();
	    length.byte[0] = getch();
#if defined __AVR_ATmega128__
	    if (address.word>0x7FFF) flags.rampz = 1;		// No go with m256, FIXME
	    else flags.rampz = 0;
#endif
	    if (getch() == 'E') flags.eeprom = 1;
	    else {
		flags.eeprom = 0;
		address.word = address.word << 1;	        // address * 2 -> byte location
	    }
	    if (getch() == ' ') {		                // Command terminator
		putch(0x14);
		for (w=0;w < length.word;w++) {		        // Can handle odd and even lengths okay
		    if (flags.eeprom) {	                        // Byte access EEPROM read
#if defined(__AVR_ATmega168__)
			while(EECR & (1<<EEPE));
			EEAR = (uint16_t)(void *)address.word;
			EECR |= (1<<EERE);
			putch(EEDR);
#else
			putch(eeprom_read_byte((void *)address.word));
#endif
			address.word++;
		    }
		    else {

			if (!flags.rampz) putch(pgm_read_byte_near(address.word));
#if defined __AVR_ATmega128__
			else putch(pgm_read_byte_far(address.word + 0x10000));
			// Hmmmm, yuck  FIXME when m256 arrvies
#endif
			address.word++;
		    }
		}
		putch(0x10);
	    }
	}


        /* Get device signature bytes  */
        else if(ch=='u') {
	    if (getch() == ' ') {
		putch(0x14);
		putch(SIG1);
		putch(SIG2);
		putch(SIG3);
		putch(0x10);
	    }
	}


        /* Read oscillator calibration byte */
        else if(ch=='v') {
	    byte_response(0x00);
	}


#ifdef MONITOR

	/* here come the extended monitor commands by Erik Lins */

	/* check for three times exclamation mark pressed */
	else if(ch=='!') {
	    ch = getch();
	    if(ch=='!') {
		ch = getch();
		if(ch=='!') {

#ifdef __AVR_ATmega128__
		    uint16_t extaddr;
#endif
		    uint8_t addrl, addrh;

#if defined WELCOME_MSG
		    PGM_P welcome = {WELCOME_MSG};
#else
		    PGM_P welcome = {"ATmegaBOOT Monitor " BL_RELEASE " - (C) J.P.Kyle, E.Lins, JD\n\r"};
#endif

		    /* turn on LED */
		    LED_DDR |= _BV(LED);
		    LED_PORT &= ~_BV(LED);

		    /* print a welcome message and command overview */
		    putsP(welcome);

		    /* test for valid commands */
		    for(;;)
		    {
			putsP(PSTR("\n\r: "));

			ch = getch();
			putch(ch);

			/* toggle LED */
			if(ch == 't') {
			    if(bit_is_set(LED_PIN,LED)) {
				LED_PORT &= ~_BV(LED);
				putch('1');
			    } else {
				LED_PORT |= _BV(LED);
				putch('0');
			    }

			}

			/* read byte from address */
			else if(ch == 'r') {
			    ch = getch(); putch(ch);
			    addrh = gethex();
			    addrl = gethex();
			    putch('=');
			    ch = *(uint8_t *)((addrh << 8) + addrl);
			    puthex(ch);
			}

			/* write a byte to address  */
			else if(ch == 'w') {
			    ch = getch(); putch(ch);
			    addrh = gethex();
			    addrl = gethex();
			    ch = getch(); putch(ch);
			    ch = gethex();
			    *(uint8_t *)((addrh << 8) + addrl) = ch;
			}

			/* dump bootloader pin to check it */
                        else if (ch == 'p') {
#ifdef BL_PIN
			    putsP(PSTR("BL="));
			    if(bit_is_set(BL_PIN, BL)) {
				putch('1');
			    } else {
				putch('0');
			    }
#endif
                        }

			/* read from uart and echo back */
			else if(ch == 'u') {
			    for(;;) {
				putch(getch());
			    }
			}
#ifdef __AVR_ATmega128__
			/* external bus loop  */
			else if(ch == 'b') {
			    putsP(PSTR("bus"));
			    MCUCR = 0x80;
			    XMCRA = 0;
			    XMCRB = 0;
			    extaddr = 0x1100;
			    for(;;) {
				ch = *(volatile uint8_t *)extaddr;
				if(++extaddr == 0) {
				    extaddr = 0x1100;
				}
			    }
			}
#endif

			else if(ch == 'j') {
			    app_start();
			}

		    }
		    /* end of monitor functions */

		}
	    }
	}
	/* end of monitor */
#endif

    }
    /* end of forever loop */

}


void putsP (PGM_P s)
{
    for(i=0; s[i] != '\0'; ++i)
    {
	putch(s[i]);
    }
}


char gethex(void)
{
    char ah,al;

    ah = getch(); putch(ah);
    al = getch(); putch(al);
    if(ah >= 'a') {
	ah = ah - 'a' + 0x0a;
    } else if(ah >= '0') {
	ah -= '0';
    }
    if(al >= 'a') {
	al = al - 'a' + 0x0a;
    } else if(al >= '0') {
	al -= '0';
    }
    return (ah << 4) + al;
}


void puthex(char ch)
{
    char ah,al;

    ah = (ch & 0xf0) >> 4;
    if(ah >= 0x0a) {
	ah = ah - 0x0a + 'a';
    } else {
	ah += '0';
    }
    al = (ch & 0x0f);
    if(al >= 0x0a) {
	al = al - 0x0a + 'a';
    } else {
	al += '0';
    }
    putch(ah);
    putch(al);
}


void putch(char ch)
{
#ifdef RS485_TXON
    RS485_PORT |= _BV(RS485_TXON);	     /* enable RS485 transmitter */
#endif

#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega324P__)
    if(bootuart == 0) {
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = ch;
	#ifdef RS485_TXON
	while (!(UCSR0A & _BV(UDRE0)));
	while (!(UCSR0A & _BV(TXC0)));
	UCSR0A |= _BV(TXC0);
	#endif
    }
    else if (bootuart == 1) {
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = ch;
	#ifdef RS485_TXON
	while (!(UCSR1A & _BV(UDRE1)));
	while (!(UCSR1A & _BV(TXC1)));
	UCSR1A |= _BV(TXC1);
	#endif
    }
#else
    /* m8,16,32,169,8515,8535,163 */
    while (!(UCSRA & _BV(UDRE)));
    UDR = ch;
    #ifdef RS485_TXON
    while (!(UCSRA & _BV(UDRE)));
    while (!(UCSRA & _BV(TXC)));
    UCSRA |= _BV(TXC);
    #endif
#endif

    #ifdef RS485_TXON
    RS485_PORT &= ~_BV(RS485_TXON);	     /* disable RS485 transmitter */
    #endif
}


char getch(void)
{
    uint8_t d, s;
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega324P__)
    if(bootuart == 0) {
	while(!(UCSR0A & _BV(RXC0)));
	s = UCSR0A; d = UDR0;
	if ( !((s&_BV(FE0))|(s&_BV(DOR0))) )
	    return d;
    }
    else if(bootuart == 1) {
	while(!(UCSR1A & _BV(RXC1)));
	s = UCSR1A; d = UDR1;
	if ( !((s&_BV(FE1))|(s&_BV(DOR1))) )
	    return d;
    }
    return 0;
#else
    /* m8,16,32,169,8515,8535,163 */
    while(!(UCSRA & _BV(RXC)));
    s = UCSRA; d = UDR;
    return ((s&_BV(FE0))|(s&_BV(DOR0))) ? 0 : d;
#endif
}


void getNch(uint8_t count)
{
    uint8_t i;
    for(i=0;i<count;i++) {
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega324P__)
	if(bootuart == 0) {
	    while(!(UCSR0A & _BV(RXC0)));
	    UDR0;
	}
	else if(bootuart == 1) {
	    while(!(UCSR1A & _BV(RXC1)));
	    UDR1;
	}
#else
	/* m8,16,32,169,8515,8535,163 */
	while(!(UCSRA & _BV(RXC)));
	UDR;
#endif
    }
}


void byte_response(uint8_t val)
{
    if (getch() == ' ') {
	putch(0x14);
	putch(val);
	putch(0x10);
    }
}


void nothing_response(void)
{
    if (getch() == ' ') {
	putch(0x14);
	putch(0x10);
    }
}


/* flash onboard LED three times to signal entering of bootloader
 */
void flash_led(uint8_t count)
{
    volatile uint32_t l;		/* fool the optimizer */

    if (count == 0)
      count = 3;
    do
    {
	LED_PORT |= _BV(LED);
	for(l = 0; l < (F_CPU/500); ++l);
	LED_PORT &= ~_BV(LED);
	for(l = 0; l < (F_CPU/800); ++l);
    } while (--count);
}


#ifdef WANT_WAIT_BL
void wait_bl_pin (void)
{
    volatile uint32_t l;		/* fool the optimizer */
    do
    {
	LED_PORT |= _BV(LED);
	for(l = 0; l < (F_CPU/200); ++l);
	LED_PORT &= ~_BV(LED);
	for(l = 0; l < (F_CPU/200); ++l);
    } while ( !bit_is_set(BL_PIN, BL) );
    for(l = 0; l < (F_CPU/200); ++l);
}
#endif

/* ==[End of file]========================================================== */
