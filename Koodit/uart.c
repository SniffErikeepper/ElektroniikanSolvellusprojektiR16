#include "uart.h"
#include <avr/io.h>

void uart_init(unsigned int ubrr) {
    UBRR0H = (unsigned char) (ubrr >> 8);
    UBRR0L = (unsigned char) ubrr; // baudinopeuden rekisterit

    UCSR0B = (1 << RXEN0) | (1 << TXEN0); //  TX & RX päälle
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // framen koko 8 bittiä
}

void uart_putchar(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}