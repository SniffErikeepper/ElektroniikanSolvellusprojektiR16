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

#define LED_PIN PD4 // debuggia varten vaan. poistetaan lopullisesta

volatile uint8_t btn_state = 1; 
volatile uint8_t btn_event = 0;

int main(void) {
    // setuppi
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT0);
    DDRD |= (1 << LED_PIN); // PD0 pinni outputiksi
    DDRB &= ~(1 << DDB0); // rekisteriin tuolle bitille 0, nappipinni inputiksi
    PORTB |= (1 << PORTB0); // ylösvetovastus päälle
    uart_init(UBRR_VALUE);
    adc_init();
    spi_init();
    st7735_init();
    char buffer[20];
    sei(); // tästä eteenpäin sallitaan keskytykset
    
    //ikiluuppi
    while (1) {

        if(btn_event){
            btn_event = 0;
            _delay_ms(20); // debounce odottelu

            uint8_t current = PINB & (1 << PINB0);

            if(current != btn_state){
                btn_state = current;

                if(!(current)){
                    uint16_t rms = adc_to_rms();
                    float db = rms_to_db(rms);
                    dtostrf(db, 6, 2, buffer);
                    st7735_draw_text(5, 30, "Ruutu toimii\nT: Antti\n juon kaljan \n", &FreeSans, 1, ST7735_COLOR_CYAN);

                }
            }

        }


    }
    return 0;
}

ISR(PCINT0_vect){ 
    btn_event = 1;
}