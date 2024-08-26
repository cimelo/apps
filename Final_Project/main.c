#include "traffic.h"

static THD_WORKING_AREA(wa_thd_serial, 32);
static THD_FUNCTION(thd_serial, arg) {
	(void) arg;

	chRegSetThreadName("Serial");

	while(1) {
		chMtxLock(&mtx_io);

		if ( sdReadI(&SD1, buffer_cmd, BUFF_LEN) ) {
			//sdWrite(&SD1, "\r\n", 3);
			//sdWrite(&SD1, buffer_cmd, BUFF_LEN);
			process_cmd();
			flush_buffer();
		}

		chMtxUnlock(&mtx_io);

		chThdSleepMilliseconds(100);
	}
};

int main(void) {
	halInit();
	chSysInit();
	sdStart(&SD1, NULL);

	// Mutex initialization
	chMtxObjectInit(&mtx_io);
	chThdCreateStatic(wa_thd_serial, sizeof(wa_thd_serial),\
				   NORMALPRIO+1, thd_serial, NULL);

	init_program();

	print_vias();

	while (1) ;
}
