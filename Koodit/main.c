#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "adc.h"
#include "uart.h"
#include "spi.h"
#include "st7735.h"
#include "st7735_gfx.h"
#include "st7735_font.h"
#include "free_sans.h"

//#define LED_PIN PD4 // debuggia varten vaan. poistetaan lopullisesta

volatile uint8_t btn_state = 1; 
volatile uint8_t btn_event = 0;

volatile float RMS_ref;
volatile float db_ref; 

int main(void) {
    // setuppi
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT0);
   // DDRD |= (1 << LED_PIN); // PD0 pinni outputiksi
    DDRB &= ~(1 << DDB0); // rekisteriin tuolle bitille 0, nappipinni inputiksi
    PORTB |= (1 << PORTB0); // ylösvetovastus päälle
    uart_init(UBRR_VALUE);
    adc_init();
    spi_init();
    st7735_init();
    char buffer[20];
    char refState[20];
    char adcValue[20];
    sei(); // tästä eteenpäin sallitaan keskytykset
    st7735_fill_rect(0,0,128,160,ST7735_COLOR_BLACK);

    typedef enum {LOW, MID, HIGH} RefLevel; //Luodaan enum eri mittaustasoille (eri desibeli referenssit) 

    volatile RefLevel curRefLevel = LOW;

    while(1){
        if(btn_event){   //Ainakun nappi keskeytys tapahtuu mennään seuraavalle tasolle LOW->MID->HIGH->LOW
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
                db_ref = 10.0;
                RMS_ref = 1.0; //ei vielä kalibroitu
                break;
            case MID:
                db_ref = 50.0;
                RMS_ref = 50.0;
                break;
            case HIGH:
                db_ref = 85.0;
                RMS_ref = 100.0; //ei vielä kalibroitu
                break;
        }

        uint16_t rms = adc_to_rms();
        float db = rms_to_db(rms);
        dtostrf(db, 6, 2, buffer); // muuttaa db:n(float) stringiksi
        dtostrf(curRefLevel + 1, 6, 2, refState);
        dtostrf(rms, 6, 2, adcValue);
        st7735_draw_text(5, 30, "Desibelit: ",&FreeSans, 1, ST7735_COLOR_CYAN);
        st7735_draw_text(5, 50, buffer,&FreeSans, 1, ST7735_COLOR_CYAN);
        st7735_draw_text(5, 110, "ADC value: ",&FreeSans, 1, ST7735_COLOR_RED);
        st7735_draw_text(5, 130, adcValue,&FreeSans, 1, ST7735_COLOR_BLUE);
        st7735_draw_text(5, 70, "Level: ",&FreeSans, 1, ST7735_COLOR_RED);
        st7735_draw_text(1, 90, refState,&FreeSans, 1, ST7735_COLOR_BLUE);
        _delay_ms(100);
        st7735_fill_rect(0,0,128,160,ST7735_COLOR_BLACK);
    }
}

ISR(PCINT0_vect) {
    if (!(PINB & (1 << PINB0))) {
        btn_event = 1;
    }
}
    

   