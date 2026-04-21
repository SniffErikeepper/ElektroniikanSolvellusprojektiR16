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

volatile uint8_t btn_state = 1; 
volatile uint8_t btn_event = 0;
volatile float maxdb = 0;


int main(void) {
    // setuppi
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT9);
    DDRC &= ~(1 << DDC1); // rekisteriin tuolle bitille 0, nappipinni inputiksi
    PORTC |= (1 << PORTC1); // ylösvetovastus päälle
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
                    while(1){
                        if(btn_event) break;
                    }
                    
                    break;
                }
            }
        }
    }
}

ISR(PCINT1_vect) {
    if (!(PINC & (1 << PINC1))) {
        btn_event = 1;
    }
}
    

   