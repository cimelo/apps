#include "hal.h"
#include "ch.h"
#include "liquidcrystal.h"

/*
 * LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(wa_write, 32);
static THD_FUNCTION(thd_write, arg) {

	(void)arg;

	chRegSetThreadName("LCD");

	while (true) {


	}
}

/*
 * Application entry point.
 */
int main(void) {
	halInit();
	chSysInit();


	sdStart(&SD1, NULL);

	lcd.begin();
	lcd.print("Teste!");

	chThdCreateStatic(wa_lcd, sizeof(wa_lcd), NORMALPRIO+1, thd_lcd, NULL);

	while (1) ;
}
