/*
 * This library sends data to the LCD module and sets
 * the cursor position. It uses 4-bit mode to save pins,
 * as the Arduino Uno has limited pins. While 8-bit mode
 * would save some CPU cycles, the pin economy is more
 * important on this platform, making 4-bit mode the
 * better choice.
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

#define DISPLAY_CTRL 0x08
#define DISPLAY_HOME 0x02
#define DISPLAY_ON 0x04
#define DISPLAY_OFF 0x00
#define DISPLAY_SHIFT 0x08
#define DISPLAY_CLEAR 0x01

#define SHIFT_CMD 0x10
#define SHIFT_RIGHT 0x04
#define SHIFT_LEFT 0x00

#define CURSOR_CMD 0x80
#define CURSOR_SHIFT 0x00
#define CURSOR_ON 0x02
#define CURSOR_OFF 0x00
#define CURSOR_BLINK_ON 0x01
#define CURSOR_BLINK_OFF 0x00

#define DL_4BITS 0x00
#define DL_8BITS 0x10
#define ROWS_1 0x00
#define ROWS_2 0x08
#define FONT_8 0x00
#define FONT_10 0x04
#define DOTS_8 0x00
#define DOTS_10 0x08

// We declare data using Little Endian format and
// we use the digital pins signature as they are
// written on the Arduino Board
struct Lcd {
	uint8_t rs;
	uint8_t en;
	uint8_t rows;
	uint8_t cols;
	uint8_t data[4];
	uint8_t cursor_row;
	uint8_t cursor_col;
};

// A default configuration is given to the user
extern struct Lcd lcd;

// Helper function for user configuration
void lcd_init_4bits(uint8_t rows, uint8_t cols, uint8_t rs,\
		uint8_t e, uint8_t d4, uint8_t d5,\
		uint8_t d6, uint8_t d7);

// Follows the cycle of instructions in the datasheet of
// the Hitachi HD44780U for inialization
void lcd_begin(void);

// Function that sends the cursor to the first position
// and returns the shifted display to the original position.
// Because of it's complexity, it takes ate least 1.52ms to 
// complete
void lcd_home(void);

// Clears the display writing the character 20H into every
// position, sets the cursor into the (0, 0) position
// on the matrix, resets the DDRAM (Display Data RAM) to 0
// and sets the display into incremental mode
void lcd_clear(void);

// Sets the cursor visibility
void lcd_cursor_visibility(uint8_t is_visible);

// Turns the cursor on and then sets the cursor to blink or not
void lcd_cursor_blink(uint8_t is_blinking);

// Moves the cursor to the desired position on the 
// LCD matrix it uses the mathematical notation
// of rows in front of the cols. Note that rows and cols start
// from 0
void lcd_cursor_position(uint8_t row, uint8_t col);

// Shifts the display n times if direction is equal to 0 it 
// shifts left and right otherwise. Note that it always
// counts from the very first position and if the display is
// configured to 2 lines, both of them shift at the same time
void lcd_shift(uint8_t n, uint8_t direction);

// Sends the command to be written to the pins
void command(uint8_t cmd);

// Receives a string from the user and prints it on the LCD.
// the lcd automatically jumps to the next line if the LCD
// was configured to 2 lines and it reads 40 characters
void lcd_print(char* msg);

// Prepares the bits to be written onto the pins
void send(uint8_t byte, uint8_t op);

// Write the bits onto the pins
void write_4bits(uint8_t half_byte);

// Helper to write the bit into the pin
void digital_write(uint8_t pin, uint8_t value);

// Enable pin pulse for writing data or commands datasheet
// HD44780U fig 25 and table on page 52
void enable_pulse(void);

#endif
