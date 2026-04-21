#ifndef ADC_H
#define ADC_H
#define SAMPLES 800

#include <stdint.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>


void adc_init(void);
void adcPrintLoop(void);
uint16_t adc_read(void);
uint16_t adc_to_rms(void);
float rms_to_db(uint16_t);
extern volatile float RMS_ref;
extern volatile float db_ref;

extern volatile uint8_t btn_state; 
extern volatile uint8_t btn_event;

extern volatile float maxdb;

#endif