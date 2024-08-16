#include "ch.h"  // needs for all ChibiOS programs
#include "hal.h" // hardware abstraction layer header


// Lets configure our ADC first


// ADCConfig structure for stm32 MCUs is empty
static ADCConfig adccfg = {};

// Create buffer to store ADC results. This is
// one-dimensional interleaved array
#define ADC_BUF_DEPTH 2 // depth of buffer
#define ADC_CH_NUM 2    // number of used ADC channels
static adcsample_t samples_buf[ADC_BUF_DEPTH * ADC_CH_NUM]; // results array

// Fill ADCConversionGroup structure fields
static ADCConversionGroup adccg = {
   // this 3 fields are common for all MCUs
      // set to TRUE if need circular buffer, set FALSE otherwise
      TRUE,
      // number of channels
      (uint16_t)(ADC_CH_NUM),
      // callback function, set to NULL for begin
      NULL,
   // Resent fields are stm32 specific. They contain ADC control registers data.
   // Please, refer to ST manual RM0008.pdf to understand what we do.
      // CR1 register content, set to zero for begin
      0,
      // CR2 register content, set to zero for begin
      0,
      // SMRP1 register content, set to zero for begin
      0,
      // SMRP2 register content, set to zero for begin
      0,
      // SQR1 register content. Set channel sequence length
      ((ADC_CH_NUM - 1) << 20),
      // SQR2 register content, set to zero for begin
      0,
      // SQR3 register content. We must select 2 channels
      // For example 15th and 10th channels. Refer to the
      // pinout of your MCU to select other pins you need.
      // On STM32-P103 board they connected to PC15 and PC0 contacts
      // On STM32-103STK board they connected to EXT2-7 contact and joystick
      (15 | (10 << 5)),
};

// Thats all with configuration

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  // Setup pins of our MCU as analog inputs
  palSetPadMode(IOPORT3, 5, PAL_MODE_INPUT_ANALOG); // this is 15th channel
  palSetPadMode(IOPORT3, 0, PAL_MODE_INPUT_ANALOG); // this is 10th channel

  // Following 3 functions use previously created configuration
  // to initialize ADC block, start ADC block and start conversion.
  // &ADCD1 is pointer to ADC driver structure, defined in the depths of HAL.
  // Other arguments defined ourself earlier.
  adcInit();
  adcStart(&ADCD1, &adccfg);
  adcStartConversion(&ADCD1, &adccg, &samples_buf[0], ADC_BUF_DEPTH);

  // Thats all. Now your conversion run in background without assistance.

  uint16_t i = 0;

  while (TRUE) {
    i = samples_buf[0] / 2;
  }
  return 0;
}

// Helper function to write casted strings to SD1
void serial_write(char* msg) {
	uint8_t tmp = (uint8_t*) msg; // To use sizeof
	chnWrite(&SD1, tmp, sizeof(tmp));
}

int main(void) {
	halInit();
	chSysInit();

	sdStart(&SD1, NULL);

	chThdCreateStatic(wa_ldr, sizeof(wa_ldr), NORMALPRIO+1, thd_ldr, NULL);

	// Var that holds the conversion from float to str
	uint8_t tmp[10];

	while (true) {
		serial_write("Voltage on pin A0: ");
		sprintf(tmp, "%0.3f", voltageA0);
		serial_write(tmp);
		serial_write("\r\n");
		chThdSleepMilliseconds(500);
	}
}
