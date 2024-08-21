#include "hal.h"
#include "ch.h"
#include "liquidcrystal.h"

/*
 * LED blinker thread, times are in milliseconds.
 *
static THD_WORKING_AREA(wa_lcd, 32);
static THD_FUNCTION(thd_lcd, arg) {

	(void)arg;

	chRegSetThreadName("LCD");

	while (true) {


	}
}
*/

/*
 * Application entry point.
 */
int main(void) {
	halInit();
	chSysInit();

	sdStart(&SD1, NULL);

	lcd_begin();
	lcd_print("Teste!");

	// chThdCreateStatic(wa_lcd, sizeof(wa_lcd), NORMALPRIO+1, thd_lcd, NULL);

	while (1) ;
}
