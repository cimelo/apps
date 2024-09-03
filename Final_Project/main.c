#include "traffic.h"
#include "chprintf.h"

BaseSequentialStream* chp = (BaseSequentialStream*) &SD1;

static THD_WORKING_AREA(wa_thd_serial, 32);
static THD_FUNCTION(thd_serial, arg);

static THD_WORKING_AREA(wa_thd_lcd, 32);
static THD_FUNCTION(thd_lcd, arg);

// Simulation global clock
static THD_WORKING_AREA(wa_thd_clk, 32);
static THD_FUNCTION(thd_clk, arg);

static THD_WORKING_AREA(wa_thd_sem_timer, 32);
static THD_FUNCTION(thd_sem_timer, arg);

// Semaphore logic hendling thread
static THD_WORKING_AREA(wa_thd_semaphore, 32);
static THD_FUNCTION(thd_semaphore, arg);

int main(void) {
	halInit();
	chSysInit();
	sdStart(&SD1, NULL);

	// Mutex initialization
	chMtxObjectInit(&mtx_print);
	chCondObjectInit(&cond_msg);
    chMtxObjectInit(&mtx_sem);
    chCondObjectInit(&cond_sem);

	init_program();

	chThdCreateStatic(wa_thd_serial, sizeof(wa_thd_serial),\
				   NORMALPRIO+5, thd_serial, NULL);
	chThdCreateStatic(wa_thd_lcd, sizeof(wa_thd_lcd),\
				   NORMALPRIO+1, thd_lcd, NULL);
	chThdCreateStatic(wa_thd_clk, sizeof(wa_thd_clk),\
				   NORMALPRIO+1, thd_clk, NULL);
	chThdCreateStatic(wa_thd_semaphore, sizeof(wa_thd_semaphore),\
				   NORMALPRIO+3, thd_semaphore, NULL);

	while (1) {
		chThdSleepSeconds(1);
	}
}

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

static THD_FUNCTION(thd_clk, arg) {
	(void) arg;

	chRegSetThreadName("Clk");

	while (1) {
		chMtxLock(&mtx_print);

		has_msg = 1;

		while (!has_msg) {
			chCondWait(&cond_msg);
		}

		for (uint8_t i = 0; i < N_LANES; ++i) {
			moviment(i);
		}

		chCondSignal(&cond_msg);

		chMtxUnlock(&mtx_print);

		chThdSleepSeconds(2);
	}
};

static THD_FUNCTION(thd_sem_timer, arg) {
	struct Timer_cfg *cfg = (struct Timer_cfg*) arg;

    chRegSetThreadName("Sem_Timer");

	chThdSleepSeconds(cfg->periods[0]);
	digital_write(cfg->pins[0], 0);
	for (uint8_t i = 0; i < cfg->periods[1]; ++i) {
		palTogglePad(IOPORT2, PIN(cfg->pins[1]));
		chThdSleepSeconds(1);
	}

    chMtxLock(&mtx_sem);
    is_timeout = 1;
    chCondSignal(&cond_sem);
    chMtxUnlock(&mtx_sem);

	chThdExit(MSG_OK);
}

void start_sem_timer(thread_t *tp, struct Timer_cfg *cfg) {
        chMtxLock(&mtx_sem);

		if (!tp || chThdTerminatedX(tp)) {
			tp = chThdCreateStatic(wa_thd_sem_timer,\
						  sizeof(wa_thd_sem_timer),\
						  NORMALPRIO+1,\
						  thd_sem_timer,\
						  (void*)cfg);
		}

		while (!is_timeout) {
            chCondWait(&cond_sem);
        }

        is_timeout = 0;
        chMtxUnlock(&mtx_sem);

		chThdWait(tp);
}

static THD_FUNCTION(thd_semaphore, arg) {
	(void) arg;

	chRegSetThreadName("Semaphore");

	uint8_t c = 0;
	struct Timer_cfg cfg;
	thread_t *tp = NULL;

	while (1) {
		switch (sem_handler.id_green) {
			case 0:
				cfg = (struct Timer_cfg){
					.pins = {PIN_G1, PIN_R1},
					.periods = {L1_G, L1_Y}
				};
				digital_write(PIN_G1, 1);
				digital_write(PIN_R1, 0);
				digital_write(PIN_R2, 1);
				digital_write(PIN_G2, 0);
				digital_write(PIN_RP, 1);
				digital_write(PIN_GP, 0);
				start_sem_timer(tp, &cfg);
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
				cfg = (struct Timer_cfg){
					.pins = {PIN_G2, PIN_R2},
					.periods = {L2_G, L2_Y}
				};
				digital_write(PIN_G1, 0);
				digital_write(PIN_R1, 1);
				digital_write(PIN_R2, 0);
				digital_write(PIN_G2, 1);
				start_sem_timer(tp, &cfg);
				sem_handler.id_green = 0;
				c = 0;
			break;
			case 2:
				cfg = (struct Timer_cfg){
					.pins = {PIN_GP, PIN_RP},
					.periods = {LP_G, LP_Y}
				};
				digital_write(PIN_G1, 0);
				digital_write(PIN_R1, 1);
				digital_write(PIN_GP, 1);
				digital_write(PIN_RP, 0);
				start_sem_timer(tp, &cfg);
				sem_handler.id_green = 0;
			break;
		}
	}
};

OOOOOOOOOOO
