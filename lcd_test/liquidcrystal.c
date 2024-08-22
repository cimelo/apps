#include "liquidcrystal.h"

BaseSequentialStream* chp = (BaseSequentialStream*) &SD1;

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

	// chprintf(chp, "---Begin---\r\n");

	palSetPadMode(PORT(lcd.rs), PIN(lcd.rs), PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(PORT(lcd.rs), PIN(lcd.rs));
	palSetPadMode(PORT(lcd.en), PIN(lcd.en), PAL_MODE_OUTPUT_PUSHPULL);
	for (uint8_t i = 0; i < 4; ++i) {
		palSetPadMode(PORT(lcd.data[i]), PIN(lcd.data[i]), PAL_MODE_OUTPUT_PUSHPULL);
	}

	write_4bits(0x03);
	chThdSleepMilliseconds(5);
	write_4bits(0x03);
	chThdSleepMilliseconds(5);
	write_4bits(0x03);
	chThdSleepMilliseconds(5);
	write_4bits(0x02);

	command(0x20 | ROWS_2 | DOTS_8);

	command(0x08 | DISPLAY_ON);

	command(0x04 | ENTRY_INC);

	command(DISPLAY_CLEAR);
}

void command(uint8_t cmd) {
	// chprintf(chp, "---cmd: %d---\r\n", cmd);
	send(cmd, 0);
}

void lcd_print(uint8_t* msg) {
	// chprintf(chp, "---lcd_print---\r\n");
	for (uint8_t *s = msg; *s; ++s) {
		send(*s, 1);
	}
}

void send(uint8_t byte, uint8_t op) {
	digital_write(lcd.rs, op);

	write_4bits(byte >> 4);
	write_4bits(byte);
}

void write_4bits(uint8_t half_byte) {
	for (uint8_t i = 0; i < 4; ++i) {
		digital_write(lcd.data[i], (half_byte & (1 << i)));
	}

	// chprintf(chp, "%d\r\n", half_byte);

	enable_pulse();
}

void digital_write(uint8_t pin, uint8_t value) {
	switch (value) {
		case 0:
			palClearPad( PORT(pin), PIN(pin) );
		break;
		default:
			palSetPad( PORT(pin), PIN(pin) );
	}
}

void enable_pulse(void) {
	palClearPad(PORT(lcd.en), PIN(lcd.en));
	chThdSleepMicroseconds(1);
	palSetPad(PORT(lcd.en), PIN(lcd.en));
	chThdSleepMicroseconds(1);
	palClearPad(PORT(lcd.en), PIN(lcd.en));
	// Maximum execution time for a instruction is 37us
	chThdSleepMicroseconds(100);
}
