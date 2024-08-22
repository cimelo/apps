#include "hal.h"
#include "ch.h"
#include "liquidcrystal.h"

/*
 * LED blinker thread, times are in milliseconds.
 *
 *
static THD_WORKING_AREA(wa_serial, 32);
static THD_FUNCTION(thd_serial, arg) {

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
	lcd_print("Vamor ver se agora da bom! I walk Alone, i'm not a saint! Stricken ablablabla");

	// chprintf(chp, "%d", 0x03);

	// chThdCreateStatic(wa_lcd, sizeof(wa_lcd), NORMALPRIO+1, thd_lcd, NULL);

	while (1) ;
}
