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

struct Lcd {
	uint8_t rs,
	uint8_t e,
	uint8_t rows,
	uint8_t cols,
	uint8_t data[8]
};

void init_4bits(uint8_t rs, uint8_t e, uint8_t d4,\
		uint8_t d5, uint8_t d6, uint8_t d7);

void set_cursor(struct Lcd* lcd, uint8_t row, uint8_t col);

void clear(struct Lcd* lcd, uint8_t row, uint8_t col);

void send_msg(struct Lcd* lcd, char* msg);

void write4bits(struct Lcd* lcd, uint8_t value);

void pulse(struct Lcd* lcd);

#endif
