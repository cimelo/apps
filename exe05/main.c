#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define ADC_BUFFER_DEPTH 10
#define ADC_CHANNEL_NUMBER 1

// One dimentional array used to store adc conversions
static adcsample_t adc_buffer[ADC_BUFFER_DEPTH*ADC_CHANNEL_NUMBER];

BaseSequentialStream* chp = (BaseSequentialStream*) &SD1;
static int16_t adcConvA0 = 0;
static float voltageA0 = 0;

static const ADCConversionGroup adcg = {
	0,
	ADC_CHANNEL_NUMBER,
	NULL,
	NULL,
	1,
};

static const ADCConfig adc_config = {
	ANALOG_REFERENCE_AVCC,
};

static THD_WORKING_AREA(wa_ldr, 32);
static THD_FUNCTION(thd_ldr, args) {

	chRegSetThreadName("LDR");

	adcStart(&ADCD1, &adc_config);

	while(true) {
		adcStartConversion(&ADCD1, &adcg, adc_buffer, ADC_BUFFER_DEPTH);
		for (uint8_t i = 0; i < ADC_BUFFER_DEPTH; i++) {
			adcConvA0 += adc_buffer[i];
		}

		adcConvA0 /= (ADC_CHANNEL_NUMBER * ADC_BUFFER_DEPTH);
		voltageA0 = ( ((float)adcConvA0 * 5) / 1024 ) ;
		if (voltageA0 < 2) {
			palClearPad(IOPORT4, 2);
		}
		else {
			palSetPad(IOPORT4, 2);
		}
		chThdSleepMilliseconds(100);
	}
}

int main(void) {
	halInit();
	chSysInit();

	sdStart(&SD1, NULL);

	chThdCreateStatic(wa_ldr, sizeof(wa_ldr), NORMALPRIO+1, thd_ldr, NULL);
	palSetPadMode(IOPORT4, 2, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(IOPORT4, 2);

	while (true) {
		chprintf(chp, "Voltage on pin A0: ");
		chprintf(chp, "%.3f \r\n", voltageA0);
		chThdSleepMilliseconds(500);
	}
}
