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
float dbRefArray[] = {42.0,50.0,62.5,74.0,80.0};
float rmsRefArray[] = {1.0,4.0,7.0,12.0,16.0};
volatile float curRms; //tämän hetkinen referenssi RMS arvo
volatile float curDb; //tämän hetkinen referenssi db arvo
volatile int refLevel = 0;

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
    setupMessage();

    while(1){
        if(btn_event){
            btn_state = 0;
            curDb = dbRefArray[0]; 
            curRms = rmsRefArray[0];
            while(1){  //valitsee sopivat referenssi arvot rms arvon perusteella
                uint16_t rms = adc_to_rms(); 
                float db = rms_to_db(rms);
                if(db < 40 && refLevel != 1 ){ 
                curDb = dbRefArray[0];
                curRms = rmsRefArray[0];
                refLevel = 1;
                }
                else if(db >= 40.1 && db < 55.0 && refLevel != 2){ 
                curDb = dbRefArray[1];
                curRms = rmsRefArray[1];
                refLevel = 2;
                }
                else if(db >= 55.1 && db < 70.0 && refLevel != 3 ) {
                curDb = dbRefArray[2];
                curRms = rmsRefArray[2];
                refLevel = 3;
                }
                else if(db >= 70.1 && db < 78.0 && refLevel != 4){ 
                curDb = dbRefArray[3];
                curRms = rmsRefArray[3];
                refLevel = 4;
                }
                else if(db >= 78.1 && refLevel != 5) {
                curDb = dbRefArray[4];
                curRms = rmsRefArray[4];
                refLevel = 5;
                }

                if(db > maxdb) maxdb = db; 

                adcPrint(db,rms,maxdb,refLevel);  
                _delay_ms(100);
                }
                btn_event = 0;
            }
        }
    }

ISR(PCINT1_vect) {
    if (!(PINC & (1 << PINC1))) {
        btn_event = 1;
    }
}
    

   