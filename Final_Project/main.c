#include "hal.h"
#include "ch.h"
#include "chprintf.h"
#include "traffic.h"


BaseSequentialStream* chp = (BaseSequentialStream*) &SD1;

int main(void) {
	halInit();
	chSysInit();

	sdStart(&SD1, NULL);

	lcd_init_4bits(2, 16, 6, 7, 2, 3, 4, 5);

	for (uint8_t i = 0; i < N_VIAS; ++i) {
		init_via(&vias[i]);
	}

	push(&vias[0], 'A');
	push(&vias[0], 'C');
	push(&vias[0], 'C');
	push(&vias[0], 'A');
	push(&vias[0], 'A');
	push(&vias[0], 'A');
	push(&vias[0], 'A');
	pop(&vias[0]);

	send_print(&vias[0]);

	while (1) ;
}
