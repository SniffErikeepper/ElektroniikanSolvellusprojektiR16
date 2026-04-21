#ifndef UART_H
#define UART_H

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE F_CPU/16/BAUD-1

void uart_init(unsigned int);
void uart_putchar(unsigned char);

#endif