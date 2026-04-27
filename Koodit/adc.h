#ifndef ADC_H
#define ADC_H
#define SAMPLES 10000

#include <stdint.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>


void adc_init(void);
void adcPrint(float, uint16_t, float, int);
void setupMessage(void);

uint16_t adc_read(void);
uint16_t adc_to_rms(void);
float rms_to_db(uint16_t);
extern volatile float curRms;
extern volatile float curDb; 
extern volatile float db;
extern volatile uint16_t rms;
extern volatile int refLevel;

extern volatile uint8_t btn_state; 
extern volatile uint8_t btn_event;

extern volatile float maxdb;

#endif