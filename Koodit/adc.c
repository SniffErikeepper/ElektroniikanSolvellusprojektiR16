#include "adc.h"
#include <avr/io.h>
#include <math.h>
#include <stdint.h>

#include "st7735.h"
#include "st7735_gfx.h"
#include "st7735_font.h"
#include "free_sans.h"

typedef enum {LOW, MID, HIGH} RefLevel; //Luodaan enum eri mittaustasoille (eri desibeli referenssit) 

volatile RefLevel curRefLevel = LOW;
char buffer[20];
char refState[20];
char adcValue[20];
char dbmax[20];
volatile float RMS_ref;
volatile float db_ref; 

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

void adcPrintLoop(void){
       if(btn_event){   //Aina kun nappi keskeytys tapahtuu mennään seuraavalle tasolle LOW->MID->HIGH->LOW
            _delay_ms(20); // debounce odottelu
            if (!(PINB & (1 << PINB0))) {
            if (curRefLevel == LOW) curRefLevel = MID;  
            else if (curRefLevel == MID) curRefLevel = HIGH;
            else curRefLevel = LOW; 

            st7735_fill_rect(0,0,128,160,ST7735_COLOR_BLACK);
            }
            btn_event = 0;
        }
       

        switch (curRefLevel) {  //valitaan desibli referenssi enum:in arvon perusteella. 0 = LOW jne.
            case LOW:
                db_ref = 25.0;
                RMS_ref = 25.0; //ei vielä kalibroitu
                break;
            case MID:
                db_ref = 50.0;
                RMS_ref = 50.0;
                break;
            case HIGH:
                db_ref = 75.0;
                RMS_ref = 75.0; //ei vielä kalibroitu
                break;
        }

        uint16_t rms = adc_to_rms();
        float db = rms_to_db(rms);
        dtostrf(db, 6, 2, buffer); // muuttaa db:n(float) stringiksi
        dtostrf(curRefLevel + 1, 6, 2, refState);
        dtostrf(rms, 6, 2, adcValue);
        if(db > maxdb){
            maxdb = db;
        }
        dtostrf(maxdb,6,2,dbmax);

        st7735_draw_text(5, 30, "Desibelit: ",&FreeSans, 1, ST7735_COLOR_CYAN);
        st7735_draw_text(5, 50, buffer,&FreeSans, 1, ST7735_COLOR_CYAN);
        st7735_draw_text(5, 110, "ADC value: ",&FreeSans, 1, ST7735_COLOR_RED);
        st7735_draw_text(5, 130, adcValue,&FreeSans, 1, ST7735_COLOR_BLUE);
        st7735_draw_text(5, 70, "Level: ",&FreeSans, 1, ST7735_COLOR_RED);
        st7735_draw_text(1, 90, refState,&FreeSans, 1, ST7735_COLOR_BLUE);
        st7735_draw_text(60, 70, "Max db: ",&FreeSans, 1, ST7735_COLOR_RED);
        st7735_draw_text(60, 90, dbmax,&FreeSans, 1, ST7735_COLOR_BLUE);
        _delay_ms(1500);
        st7735_fill_rect(0,0,128,160,ST7735_COLOR_BLACK);
    }
