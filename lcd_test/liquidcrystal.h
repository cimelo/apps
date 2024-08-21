/*
 *
 * This library sends data to the LCD module and sets
 * the cursor. It only uses the 4 bit mode because of
 * pin economy and for no other gain than some cpu 
 * cycles
 *
 */
#ifndef LIQUIDCRYSTAL_H_
#define LIQUIDCRYSTAL_H_

#include "hal.h"

// Helper function for digital pin definition
#define PORT(P) ( (P < 8) ? IOPORT4 : IOPORT2 )
#define PIN(P) ( (P < 8) ? P : P-8  )

// LCD 8 bit Instructions
#define ENTRYMODESET 0x04
#define ENTRY_INC 0x02
#define ENTRY_DEC 0x00
#define ENTRY_SHIFT 0x01

#define DISPLAY_ON 0x08
#define DISPLAY_OFF 0x0C
#define DISPLAY_SHIFT 0x08
#define DISPLAY_CLEAR 0x01
#define CURSOR_MOVE 0x00
#define SHIFT_RIGHT 0x04
#define SHIFT_LEFT 0x00

#define DL_8BITS 0x10
#define DL_4BITS 0x00
#define ROWS_2 0x08
#define ROWS_1 0x00
#define DOTS_10 0x08 // Dots per cell
#define DOTS_8 0x00 // Dots per cell


// We declare data using Little Endian format and
// we use the digital pins signature as they are
// written on the Arduino Board
struct Lcd {
	uint8_t rs;
	uint8_t en;
	uint8_t rows;
	uint8_t cols;
	uint8_t data[4];
};

// A default configuration is given to the user
extern struct Lcd lcd;

// Helper function for user configuration
void init_4bits(uint8_t rows, uint8_t cols, uint8_t rs,\
		uint8_t e, uint8_t d4, uint8_t d5,\
		uint8_t d6, uint8_t d7);

// Follows the cycle of instructions in the datasheet of
// the Hitachi HD44780U for inialization
void lcd_begin(void);

/*
// Clears the display
void display_clear(void);

// Set the cursor position on the LCD matrix
void set_cursor(uint8_t row, uint8_t col);
void cursor_on(void);
void cursor_off(void);
*/

// Receives a string from the user
void lcd_print(uint8_t* msg);

// Prepares the bits to be written onto the pins
static void send(uint8_t byte, uint8_t op);

// Write the bits onto the pins
static void write_4bits(uint8_t half_byte);

// Helper to write the bit into the pin
static void digital_write(uint8_t pin, uint8_t value);

// Enable pin pulse for writing data or commands datasheet
// HD44780U fig 25 and table on page 52
static void enable_pulse(void);

// Sends the command to be written to the pins
static void command(uint8_t cmd);

#endif
