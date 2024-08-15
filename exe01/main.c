#include "hal.h"
#include "ch.h"

#define LED_YLW 2
#define LED_GRN 3
#define LED_PWM 3

/*
//Called at the start of the period
static void cb_pwm_start(PWMDriver* pwmp) {
	(void) pwmp;
	palSetPad(IOPORT2, LED_PWM);
}

//Called at the end of the period
static void cb_pwm_end(PWMDriver* pwmp) {
	(void) pwmp;
	palClearPad(IOPORT2, LED_PWM);
}
*/

/*
 * Thread 1.
 */
THD_WORKING_AREA(waLed, 128);
THD_FUNCTION(Thread_Led, arg) {

	(void)arg;

	while (true) {
		palTogglePad(IOPORT4, LED_YLW);
		palTogglePad(IOPORT4, LED_GRN);
		chThdSleepMilliseconds(500);
	}
}

/*
 * Thread 2.
 */
THD_WORKING_AREA(waPwm, 128);
THD_FUNCTION(Thread_Pwm, arg) {

	(void)arg;

	while (true) {
		for (uint16_t i = 1000; i < 10000; i += 1000) {
			pwmEnableChannel(&PWMD2, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, i));
			chThdSleepMilliseconds(100);
		}
	}
}

/*
 * Threads static table, one entry per thread. The number of entries must
 * match NIL_CFG_NUM_THREADS.
 */
THD_TABLE_BEGIN
	THD_TABLE_THREAD(1, "blink_leds",  waLed,  Thread_Led,  NULL)
	THD_TABLE_THREAD(0, "pwm_led",    waPwm,  Thread_Pwm,  NULL)
THD_TABLE_END

/*
 * Application entry point.
 */
int main(void) {
	halInit();
	chSysInit();

	palSetPadMode(IOPORT4, LED_YLW, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(IOPORT4, LED_GRN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(IOPORT2, LED_PWM, PAL_MODE_OUTPUT_PUSHPULL);

	// PWM configuration

	PWMConfig pwm_driver = {
		.frequency = 1000, // Hz
		.period = 1000,     // 100ms in ticks
		.callback = NULL,
		.channels = {
			{PWM_OUTPUT_ACTIVE_HIGH, NULL},
			{PWM_OUTPUT_DISABLED, NULL}
		}
	};

	pwmStart(&PWMD2, &pwm_driver);

	while (1) ;
}
