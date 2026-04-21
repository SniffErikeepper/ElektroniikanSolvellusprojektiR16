#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "uart.h"
#include "spi.h"
#include "adc.h"
#include "st7735.h"
#include "st7735_gfx.h"
#include "st7735_font.h"

//#define LED_PIN PD4 // debuggia varten vaan. poistetaan lopullisesta

volatile uint8_t btn_state = 1; 
volatile uint8_t btn_event = 0;
volatile float maxdb = 0;


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
    sei(); // tästä eteenpäin sallitaan keskytykset
    st7735_fill_rect(0,0,128,160,ST7735_COLOR_BLUE);

    while(1){
        if(btn_event){
            int timer = 0;
            while(1){
                adcPrintLoop();  
                if(btn_event){
                    timer = 0;   
                }
                else{
                    timer++;
                }
                if(timer >= 10){
                    maxdb = 0; 
                    break;
                }
            }
        }
    }
}

ISR(PCINT0_vect) {
    if (!(PINB & (1 << PINB0))) {
        btn_event = 1;
    }
}
    

   