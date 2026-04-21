#ifndef ADC_H
#define ADC_H
#define SAMPLES 1000

#include <stdint.h>

void adc_init(void);
uint16_t adc_read(void);
uint16_t adc_to_rms(void);
float rms_to_db(uint16_t);

#endif