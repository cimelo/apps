#include "hal.h"
#include "ch.h"
#include "liquidcrystal.h"

int main(void) {
	halInit();
	chSysInit();

	sdStart(&SD1, NULL);

	lcd_begin();
	lcd_print("Hello, World!");

	while (1) ;
}
