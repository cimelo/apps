#include "traffic.h"

static THD_WORKING_AREA(wa_thd_serial, 32);
static THD_FUNCTION(thd_serial, arg) {
	(void) arg;

	chRegSetThreadName("SERIAL");

	while(1) {
		if ( sdReadI(&SD1, buffer_cmd, BUFF_LEN) ) {
			chMtxLock(&mtx_print);

			process_cmd();
			flush_buffer_cmd();

			has_msg = 1;

			chCondSignal(&cond_msg);

		}
		chMtxUnlock(&mtx_print);

		chThdSleepMilliseconds(100);
	}
};

static THD_WORKING_AREA(wa_thd_lcd, 32);
static THD_FUNCTION(thd_lcd, arg) {
	(void) arg;

	chRegSetThreadName("LCD");

	while (1) {
		chMtxLock(&mtx_print);

		while (!has_msg) {
			chCondWait(&cond_msg);
		}

		print_vias();

		has_msg = 0;

		chMtxUnlock(&mtx_print);
	}
};

// Simulation global clock
static THD_WORKING_AREA(wa_thd_clk, 32);
static THD_FUNCTION(thd_clk, arg) {
	(void) arg;

	chRegSetThreadName("CLK");

	while (1) {
		chMtxLock(&mtx_print);

		has_msg = 1;
		chCondSignal(&cond_msg);

		chMtxUnlock(&mtx_print);
		chThdSleepMilliseconds(50);

	}
};

int main(void) {
	halInit();
	chSysInit();
	sdStart(&SD1, NULL);

	// Mutex initialization
	chMtxObjectInit(&mtx_print);
	chCondObjectInit(&cond_msg);

	init_program();

	chThdCreateStatic(wa_thd_serial, sizeof(wa_thd_serial),\
				   NORMALPRIO+5, thd_serial, NULL);
	chThdCreateStatic(wa_thd_lcd, sizeof(wa_thd_lcd),\
				   NORMALPRIO+1, thd_lcd, NULL);
	chThdCreateStatic(wa_thd_clk, sizeof(wa_thd_clk),\
				   NORMALPRIO+1, thd_clk, NULL);

	while (1) ;
}
