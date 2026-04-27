#include "adc.h"
#include <avr/io.h>
#include <math.h>
#include <stdint.h>

#include "st7735.h"
#include "st7735_gfx.h"
#include "st7735_font.h"
#include "free_sans.h"
 
char buffer[20];
char refState[20];
char adcValue[20];
char dbmax[20];
char ref[20];

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
    float db = 20.0*log10(v / curRms) + curDb; // logaritmimuunnos ja referenssitaso
    return db;
}

void adcPrint(float db, uint16_t rms, float maxdb, int refLevel){
        dtostrf(db, 6, 2, buffer); // muuttaa db:n(float) stringiksi
        dtostrf(rms, 6, 2, adcValue);
        dtostrf(maxdb,6,2,dbmax);
        dtostrf(refLevel,6,2,ref);
        st7735_fill_rect(0,0,128,160,ST7735_COLOR_BLACK);
        st7735_draw_text(5, 30, "Decibels: ",&FreeSans, 1, ST7735_COLOR_CYAN);
        st7735_draw_text(5, 50, buffer,&FreeSans, 1, ST7735_COLOR_CYAN);
        st7735_draw_text(5, 110, "RMS value: ",&FreeSans, 1, ST7735_COLOR_RED);
        st7735_draw_text(5, 130, adcValue,&FreeSans, 1, ST7735_COLOR_BLUE);
        st7735_draw_text(70, 70, "Max db: ",&FreeSans, 1, ST7735_COLOR_RED);
        st7735_draw_text(70, 90, dbmax,&FreeSans, 1, ST7735_COLOR_BLUE);
        st7735_draw_text(5, 70, "Db ref: ",&FreeSans, 1, ST7735_COLOR_RED);
        st7735_draw_text(5, 90, ref,&FreeSans, 1, ST7735_COLOR_BLUE);
        //_delay_ms(1500);
        
    }

void setupMessage(void){
    st7735_fill_rect(0,0,128,160,ST7735_COLOR_BLACK);
    st7735_draw_circle(65,70,60, ST7735_COLOR_RED);
    st7735_draw_text(2, 70, "Setup complete",&FreeSans, 1, ST7735_COLOR_WHITE);
    _delay_ms(500); 
    st7735_fill_rect(0,0,128,160,ST7735_COLOR_BLACK);
    st7735_draw_circle(65,70,60, ST7735_COLOR_BLUE);
    st7735_draw_text(5, 70, "Press the \n button to start",&FreeSans, 1, ST7735_COLOR_WHITE);
};
