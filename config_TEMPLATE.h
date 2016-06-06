/* config_TEMPLATE.h -- product specific bootloader configuration header.
 *
 * This header contains declarations and defines for a special product. To
 * enable the usage of this header, uncomment USE_CONFIG_H in the
 * product_TEMPLATE.mak!
 */


/* -------------------------------------------------------------------------
 * Some macros used by this header..
 */

/* This macro converts a sequence of 0 and 1 into a BYTE. The sequence start
 * with the MSB! Make shure that you allways define all 8 bits!
 * Sample: BIN2BYTE(11001010) is converted to 0xCA (1100->C & 1010->A).
 */
#define BIN2BYTE(a) ( ((0x##a##L>>21) & 0x80) + ((0x##a##L>>18) & 0x40) \
                    + ((0x##a##L>>15) & 0x20) + ((0x##a##L>>12) & 0x10) \
                    + ((0x##a##L>>9) & 0x08) + ((0x##a##L>>6) & 0x04)  \
                    + ((0x##a##L>>3) & 0x02) + (0x##a##L & 0x01))



/* -------------------------------------------------------------------------
 * CONFIGURATION
 */



/* ===== Configure Monitor Welcome =====
 * Uncomment to define the welcome message of the Monitor.
 */
/* #define WELCOME_MSG "MegaBOOT Monitor\r\n" */


/* ===== Configure reading signature =====
 * Uncomment this to enable the UNIVERSAL command of the STK500 protocol
 * to return the signature. This method is used by AVRDUDE.
 */
/* #define WANT_AVRDUDE_SIGNATURE 1 */


/* ===== Configure the RS485 stuff =====
 * If your UART is a RS485, define the port-, the DDR-register and the pin number
 * of the output which enables (with 1) the RS485 transmitter. If you don't use a
 * RS485, comment out these three lines.
 */
/* #define RS485_TXON  PINE2 */
/* #define RS485_DDR   DDRE */
/* #define RS485_PORT  PORTE */


/* ===== Selection of the UART =====
 * If the AVR supports more than one UART, use 0 for UART0 and 1 for UART1. If
 * you comment out this define, UART0 is used as default.
 */
#define USE_UART 0


/* ===== Configure "forced enter" mode =====
 * In the case there is no bootloader pin, uncommenting this flag configures
 * the bootloader to use "forced enter". Therefore the booloader waits up to
 * 3 seconds for a sequence of 5+ consecutive '*' characters at the UART.
 * If this '*' sequence is received, the bootloader starts sending '*' and waits
 * for an empty UART buffer. If no more data is fetched, the regular bootloader
 * is entered.
 */
//#define USE_FORCED_BOOTLOAD_ENTER 1


/* ===== Configure WDT Reset handling =====
 * Uncomment the line below too ignore WDT resets. If the BL_PIN is low
 * (active), while a WDT reset occures, the bootload will ignore it and start
 * the application.
 */
//#define HANDLE_WDT_RESET 1


/* ===== Configure QUIT behaviour =====
 * After flashing the application, programmers like AVRDUDE send a 'Quit'
 * command. ATMegaBOOT normally lets the LED flash four times.
 *
 * The default behaviour is, that you have to set the BL_PIN to high and reset
 * the hardware to start the application.
 *
 * There are two different ways to handle this. Uncomment WANT_START_APP if the
 * application should be started right after the quit. Additionally, uncomment
 * WANT_WAIT_BL to wait with the automatic start of the application, until the
 * BL_PIN is high again! So WANT_WAIT_BL alone doesn't make sense.
 *
 */
//#define WANT_WAIT_BL 1
//#define WANT_START_APP 1


/* ===== Configuration of the port of the MCU =====
 *
 * To avoid hardware damage while the bootloader is active, it's possible to
 * define the DDR and PORT register of every port of the MCU. Select the
 * port which must have another than the powerup defaults.
 *
 * To enable the setup of an port, uncomment INIT_PORT_*!
 */

/* #define INIT_PORT_A */
/* #define DIR_PORT_A  BIN2BYTE(00000000) */
/* #define VAL_PORT_A  BIN2BYTE(11111111) */

/* #define INIT_PORT_B */
/* #define DIR_PORT_B  BIN2BYTE(00000000) */
/* #define VAL_PORT_B  BIN2BYTE(11111111) */

/* #define INIT_PORT_C */
/* #define DIR_PORT_C  BIN2BYTE(00000000) */
/* #define VAL_PORT_C  BIN2BYTE(11111111) */

/* #define INIT_PORT_D */
/* #define DIR_PORT_D  BIN2BYTE(00000000) */
/* #define VAL_PORT_D  BIN2BYTE(11111111) */

/* #define INIT_PORT_E */
/* #define DIR_PORT_E  BIN2BYTE(00000000) */
/* #define VAL_PORT_E  BIN2BYTE(11111111) */

/* #define INIT_PORT_F */
/* #define DIR_PORT_F  BIN2BYTE(00000000) */
/* #define VAL_PORT_F  BIN2BYTE(11111111) */

/* #define INIT_PORT_G */
/* #define DIR_PORT_G  BIN2BYTE(00000000) */
/* #define VAL_PORT_G  BIN2BYTE(11111111) */

/* #define INIT_PORT_H */
/* #define DIR_PORT_H  BIN2BYTE(00000000) */
/* #define VAL_PORT_H  BIN2BYTE(11111111) */



/* ==[End of file]========================================================== */
