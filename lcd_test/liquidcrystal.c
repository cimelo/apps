#include "liquidcrystal.h"

// Default configurations
struct Lcd lcd = {
	.rows = 2,
	.cols = 16,
	.rs = 8,
	.en = 9,
	.data = {4, 5, 6, 7}
};

void init_4bits(uint8_t rows, uint8_t cols, uint8_t rs,\
		uint8_t en, uint8_t d4, uint8_t d5,\
		uint8_t d6, uint8_t d7) {

	lcd = (struct Lcd){
		.rows = rows,
		.cols = cols,
		.rs = rs,
		.en = en,
		.data = {d4, d5, d6, d7}
	};

	lcd_begin();
}

void lcd_begin(void) {
	chThdSleepMilliseconds(50);

	command(0x30);
	chThdSleepMilliseconds(5);
	command(0x30);
	chThdSleepMilliseconds(5);
	command(0x30);
	chThdSleepMilliseconds(5);

	command(0x20);

	command(0x20 | ROWS_2 | DOTS_8);

	command(DISPLAY_OFF);

	command(DISPLAY_CLEAR | ENTRY_INC | ENTRY_SHIFT);
}

static void command(uint8_t cmd) {
	send(cmd, 0);
}

void lcd_print(uint8_t* msg) {
	for (uint8_t *s = msg; *s; ++s) {
		send(*s, 1);
	}
}

static void send(uint8_t byte, uint8_t op) {
	lcd.rs = op;

	write_4bits(byte >> 4);
	write_4bits(byte);
}

static void write_4bits(uint8_t half_byte) {
	for (uint8_t i = 0; i < 4; ++i) {
		digital_write(lcd.data[i], (half_byte & (1 << i)));
	}

	enable_pulse();
}

static void digital_write(uint8_t pin, uint8_t value) {
	switch (value) {
		case 0:
			palClearPad( PORT(pin), PIN(pin) );
		break;
		default:
			palSetPad( PORT(pin), PIN(pin) );
	}
}

static void enable_pulse(void) {
	palClearPad(IOPORT2, lcd.en);
	chThdSleepMicroseconds(1);
	palSetPad(IOPORT2, lcd.en);
	chThdSleepMicroseconds(1);
	palClearPad(IOPORT2, lcd.en);
	// Maximum execution time for a instruction is 37us
	chThdSleepMicroseconds(100);
}
