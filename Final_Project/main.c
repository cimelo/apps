#include "traffic.h"

static THD_WORKING_AREA(wa_thd_serial, 32);
static THD_FUNCTION(thd_serial, arg) {
	(void) arg;

	chRegSetThreadName("Serial");

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

	chRegSetThreadName("Lcd");

	while (1) {
		chMtxLock(&mtx_print);

		while (!has_msg) {
			chCondWait(&cond_msg);
		}

		print_lanes();

		has_msg = 0;

		chMtxUnlock(&mtx_print);
	}
};

// Simulation global clock
static THD_WORKING_AREA(wa_thd_clk, 128);
static THD_FUNCTION(thd_clk, arg) {
	(void) arg;

	chRegSetThreadName("Clk");

	while (1) {
		chMtxLock(&mtx_print);

		has_msg = 1;

		for (uint8_t i = 0; i < N_LANES; ++i) {
			moviment(i);
		}

		chCondSignal(&cond_msg);

		chMtxUnlock(&mtx_print);

		chThdSleepMilliseconds(2000);
	}
};

// Semaphore logic hendling thread
static THD_WORKING_AREA(wa_thd_semaphore, 256);
static THD_FUNCTION(thd_semaphore, arg) {
	(void) arg;

	chRegSetThreadName("Semaphore");

	uint8_t c = 0;

	while (1) {
		switch (sem_handler.id_green) {
			case 0:
				digital_write(PIN_G1, 1);
				digital_write(PIN_R1, 0);
				digital_write(PIN_R2, 1);
				digital_write(PIN_G2, 0);
				digital_write(PIN_RP, 1);
				digital_write(PIN_GP, 0);
				chThdSleepSeconds(L1_G);
				if(lanes[2].n && !c) {
					sem_handler.id_green = 2;
					c = 1;
				}
				else if (lanes[1].n) {
					sem_handler.id_green = 1;
				}
				else {
					c = 0;
				}
			break;
			case 1:
				digital_write(PIN_G1, 0);
				digital_write(PIN_R1, 1);
				digital_write(PIN_R2, 0);
				digital_write(PIN_G2, 1);
				chThdSleepSeconds(L2_G);
				sem_handler.id_green = 0;
				c = 0;
			break;
			case 2:
				digital_write(PIN_G1, 0);
				digital_write(PIN_R1, 1);
				digital_write(PIN_GP, 1);
				digital_write(PIN_RP, 0);
				chThdSleepSeconds(LP_G);
				sem_handler.id_green = 0;
			break;
		}
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
	chThdCreateStatic(wa_thd_semaphore, sizeof(wa_thd_semaphore),\
				   NORMALPRIO+3, thd_semaphore, NULL);

	while (1) ;
}
