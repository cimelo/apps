#include "hal.h"
#include "ch.h"
#include "liquidcrystal.h"
#include "chprintf.h"
#include "traffic.h"

BaseSequentialStream* chp = (BaseSequentialStream*) &SD1;

int main(void) {
	halInit();
	chSysInit();

	sdStart(&SD1, NULL);

	lcd_init_4bits(2, 16, 6, 7, 2, 3, 4, 5);

	while (1) {
		chprintf(chp, "");
	}
}
