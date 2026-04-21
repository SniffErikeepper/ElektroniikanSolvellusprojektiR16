#include "adc.h"
#include <avr/io.h>
#include <math.h>
#include <stdint.h>



void adc_init(void){

    ADMUX = (1 << REFS0);

    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    DIDR0 = (1 << ADC0D);
}

uint16_t adc_read(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

uint16_t adc_to_rms(){
    uint32_t sum = 0;

    for(int i = 0; i < SAMPLES; i++){  
        int16_t sample = adc_read();
        sample -= 512;  // poistetaan lukemasta DC offset
        sum += (uint32_t)(sample*sample); // lukemat neliöidään ja summataan
    }
    uint16_t mean = sum / SAMPLES; // summien keskiarvo
    uint16_t rms = sqrt(mean);    // ja keskiarvosta neliö
    return rms;
}

float rms_to_db(uint16_t rms){   // tarvii parantelua
    float v = (float)rms;
    if(v < 1) v = 1;
    float db = 20.0*log10(v / RMS_ref) + db_ref; // logaritmimuunnos ja referenssitaso
    return db;
}