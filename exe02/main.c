#include "hal.h"
#include "ch.h"

#define LED_YLW 2
#define LED_GRN 3
#define LED_PWM 3

/*
 * Thread 1.
 */
THD_WORKING_AREA(wa_Led, 128);
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
THD_WORKING_AREA(wa_SerComm, 128);
THD_FUNCTION(Thread_SerComm, arg) {

	(void)arg;

	uint8_t msg[] = "Hello, World!\r\n";

	while (true) {
		sdWrite(&SD1, msg, sizeof(msg));
		chThdSleepMilliseconds(500);
	}
}

/*
 * Thread 3.
 */
THD_WORKING_AREA(wa_Pwm, 128);
THD_FUNCTION(Thread_Pwm, arg) {

	(void)arg;

	while (true) {
		for (uint16_t i = 1000; i < 10000; i += 1000) {
			pwmEnableChannel(&PWMD2, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, i));
			chThdSleepMilliseconds(500);
		}
	}
}

/*
 * Threads static table, one entry per thread. The number of entries must
 * match NIL_CFG_NUM_THREADS.
 */
THD_TABLE_BEGIN
	THD_TABLE_THREAD(2, "pwm", wa_Pwm, Thread_Pwm,  NULL)
	THD_TABLE_THREAD(1, "blink_leds", wa_Led, Thread_Led,  NULL)
	THD_TABLE_THREAD(0, "pwm_ser_comm", wa_SerComm, Thread_SerComm,  NULL)
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

	// Serial Config
	SerialConfig driver_serial = {
		.sc_brr = UBRR2x(9600),
		.sc_bits_per_char = USART_CHAR_SIZE_8
	};

	sdStart(&SD1, &driver_serial);

	// PWM Config
	PWMConfig driver_pwm = {
		.frequency = 1000,
		.period = 100,
		.callback = NULL,
		.channels = {
			{PWM_OUTPUT_ACTIVE_HIGH, NULL},
			{PWM_OUTPUT_DISABLED, NULL}
		}
	};

	pwmStart(&PWMD2, &driver_pwm);

	while (1) ;
}
