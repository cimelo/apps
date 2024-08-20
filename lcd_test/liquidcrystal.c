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
		uint8_t e, uint8_t d4, uint8_t d5,\
		uint8_t d6, uint8_t d7) {

	lcd = {
		.rows = rows,
		.cols = cols,
		.rs = rs,
		.en = en,
		.data = {d4, d5, d6, d7},
	}

	begin();
}

void begin(void) {
	chThreadSleepMilliseconds(50);

	command(0x03);
}

void command(uint8_t cmd) {
	send(cmd, 0);
}

void send_byte(uint8_t byte, uint8_t op) {
	lcd.rs = op;

	write_4bits(byte >> 4);
	write_4bits(byte);
}

void write_4bits(unit8_t half_byte) {
	for (uint8_t i = 0; i < 4; ++i) {
		digital_write(lcd.data[i], (half_byte & (1 << i)));
	}

	enable_pulse();
}

void digital_write(uint8_t pin, uint8_t value) {
	switch (value) {
		case 0:
			palClearPad(PORT(pin), PIN(pin));
		break;
		default:
			palSetPad(PORT(pin), PIN(pin));
	}
}

void print(uint8_t* msg) {
	for (uint8_t *s = msg; *s; ++s) {
		send(*s, 1);
	}
}

void enable_pulse(void) {
	palClearPad(IOPORTB, ldc.en);
	chThreadSleepMicroseconds(1);
	palSetPad(IOPORTB, ldc.en);
	chThreadSleepMicroseconds(1);
	palClearPad(IOPORTB, ldc.en);
	// Maximum execution time for a instruction is 37us
	chThreadSleepMicroseconds(100);
}
