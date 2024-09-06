#include "traffic.h"
#include "chprintf.h"

BaseSequentialStream* chp = (BaseSequentialStream*) &SD1;

thread_t *tp_sem = NULL;
binary_semaphore_t amb_sem;
thread_t *tp_amb1 = NULL;
thread_t *tp_amb2 = NULL;

// Thread responsible for receiving commands
// via serial communication
static THD_WORKING_AREA(wa_thd_serial, 32);
static THD_FUNCTION(thd_serial, arg);

// Thread responsible for displaying traffic
// information on the LCD
static THD_WORKING_AREA(wa_thd_lcd, 32);
static THD_FUNCTION(thd_lcd, arg);

// Thread responsible for managing the simulation's
// global clock
static THD_WORKING_AREA(wa_thd_clk, 32);
static THD_FUNCTION(thd_clk, arg);

// Thread responsible for signaling semaphore timers
// upon timeout
static THD_WORKING_AREA(wa_thd_sem_timer, 32);
static THD_FUNCTION(thd_sem_timer, arg);

// Semaphore logic hendling thread
static THD_WORKING_AREA(wa_thd_semaphore, 32);
static THD_FUNCTION(thd_semaphore, arg);

// Timer set up when an ambulance is detected
// on the primary lane
static THD_WORKING_AREA(wa_thd_wait_amb1, 32);
static THD_FUNCTION(thd_wait_amb1, arg);

// Timer set up when an ambulance is detected on
// the secondary lane, provided there is no
// ambulance on the primary lane
static THD_WORKING_AREA(wa_thd_wait_amb2, 32);
static THD_FUNCTION(thd_wait_amb2, arg);


// ================ Main ======================

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

// ================ Thread Implementations ======================

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

		if (sem_handler.id_green != 0 && lanes[0].n_ambs &&\
			(!tp_amb1 || chThdTerminatedX(tp_amb1)) ) {
			tp_amb1 = chThdCreateStatic(wa_thd_wait_amb1,\
						  sizeof(wa_thd_wait_amb1),\
						  NORMALPRIO+1,\
						  thd_wait_amb1,\
						  NULL);
		}
		if (sem_handler.id_green != 1 && lanes[1].n_ambs &&\
			(!tp_amb2 || chThdTerminatedX(tp_amb2)) ) {
			tp_amb2 = chThdCreateStatic(wa_thd_wait_amb2,\
						  sizeof(wa_thd_wait_amb2),\
						  NORMALPRIO+1,\
						  thd_wait_amb2,\
						  NULL);
		}

		chThdSleepMilliseconds(100);
	}
};

static THD_FUNCTION(thd_wait_amb1, arg) {
	(void) arg;

	chRegSetThreadName("Clk");

	chprintf(chp, "TA1!\r\n");

	chThdSleepSeconds(AMB_WAIT);

	chMtxLock(&mtx_sem);
	if (sem_handler.id_green != 0 && lanes[0].n_ambs) {
		chBSemSignal(&amb_sem);
		is_timeout = 1;
		sem_handler.interr = 1;
		sem_handler.id_green = 0;
		chCondSignal(&cond_sem);
	}
	chMtxUnlock(&mtx_sem);

	chThdExit(MSG_OK);
};

static THD_FUNCTION(thd_wait_amb2, arg) {
	(void) arg;

	chprintf(chp, "TA2!\r\n");

	chRegSetThreadName("Clk");

	chThdSleepSeconds(AMB_WAIT);
	
	while (lanes[0].n_ambs) {
		chThdSleepMilliseconds(100);
	}

	chMtxLock(&mtx_sem);
	if (sem_handler.id_green != 1 && lanes[1].n_ambs) {
		chBSemSignal(&amb_sem);
		is_timeout = 1;
		sem_handler.interr = 1;
		sem_handler.id_green = 1;
		chCondSignal(&cond_sem);
	}
	chMtxUnlock(&mtx_sem);

	chThdExit(MSG_OK);
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

	uint16_t i = 0, end = 0;

	for (i = 0; i < cfg->periods[0]*10; ++i) {
		if (chBSemWaitTimeout(&amb_sem, TIME_IMMEDIATE) == MSG_OK) {
			end = 1;
			break;
		}
		chThdSleepMilliseconds(100);
	}
	palClearPad(IOPORT2, PIN(cfg->pins[0]));
	if (lanes[1].n || lanes[2].n) {
		for (i = 0; !end && i < cfg->periods[1]*10; ++i) {
			if (chBSemWaitTimeout(&amb_sem, TIME_IMMEDIATE) == MSG_OK) {
				break;
			}
			if (i%10 == 0) {
				palTogglePad(IOPORT2, PIN(cfg->pins[1]));
			}
			chThdSleepMilliseconds(100);
		}
	}

    chMtxLock(&mtx_sem);
    is_timeout = 1;
    chCondSignal(&cond_sem);
    chMtxUnlock(&mtx_sem);

	chThdExit(MSG_OK);
}

void start_sem_timer(thread_t *tp, struct Timer_cfg *cfg) {
		if (!tp || chThdTerminatedX(tp)) {
			tp = chThdCreateStatic(wa_thd_sem_timer,\
						  sizeof(wa_thd_sem_timer),\
						  NORMALPRIO+1,\
						  thd_sem_timer,\
						  (void*)cfg);
		}
					
					        chMtxLock(&mtx_sem);

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
				start_sem_timer(tp_sem, &cfg);
				if (!lanes[0].n_ambs) {
					if( lanes[2].n && !c ) {
						chMtxLock(&mtx_sem);
						if (!sem_handler.interr) {
							sem_handler.id_green = 2;
						}
						else {
							sem_handler.interr = 0;
						}
						c = 1;
						chMtxUnlock(&mtx_sem);
					}
					else if (lanes[1].n) {
						chMtxLock(&mtx_sem);
						if (!sem_handler.interr) {
							sem_handler.id_green = 1;
						}
						else {
							sem_handler.interr = 0;
						}
						chMtxUnlock(&mtx_sem);
					}
				}
				

				if (!lanes[1].n){
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
				digital_write(PIN_GP, 0);
				digital_write(PIN_RP, 1);
				start_sem_timer(tp_sem, &cfg);
				chMtxLock(&mtx_sem);
				if (!sem_handler.interr) {
					sem_handler.id_green = 0;
				}
				else {
					sem_handler.interr = 0;
				}
				c = 0;
				chMtxUnlock(&mtx_sem);
			break;
			case 2:
				cfg = (struct Timer_cfg){
					.pins = {PIN_GP, PIN_RP},
					.periods = {LP_G, LP_Y}
				};
				digital_write(PIN_G1, 0);
				digital_write(PIN_R1, 1);
				digital_write(PIN_G2, 0);
				digital_write(PIN_R2, 1);
				digital_write(PIN_GP, 1);
				digital_write(PIN_RP, 0);
				start_sem_timer(tp_sem, &cfg);
				chMtxLock(&mtx_sem);
				if (!sem_handler.interr) {
					if (lanes[1].n){
						sem_handler.id_green = 1;
					}
					else {
						sem_handler.id_green = 0;
					}
				}
				else {
					sem_handler.interr = 0;
				}
				chMtxUnlock(&mtx_sem);
			break;
		}
	}
};
