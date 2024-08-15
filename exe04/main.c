#include "ch.h"
#include "hal.h"

/*
 * LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 32);
static THD_FUNCTION(Thread1, arg) {

	chRegSetThreadName("Blinker1");

	while (true) {
		palTogglePad(IOPORT2, 5);
		chThdSleepMilliseconds((*(uint16_t*) arg));
	}
}

static THD_WORKING_AREA(waThread2, 32);
static THD_FUNCTION(Thread2, arg) {

	chRegSetThreadName("Blinker2");

	while (true) {
		palTogglePad(IOPORT2, 5);
		chThdSleepMilliseconds((*(uint16_t*)arg)*7);
	}
}

/*
 * Application entry point.
 */
int main(void) {
	halInit();
	chSysInit();

	palSetPadMode(IOPORT2, 5, PAL_MODE_OUTPUT_PUSHPULL);

	uint16_t period = 500;

	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, (void*)&period);
	chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, (void*)&period);

	while (true) ;
}
