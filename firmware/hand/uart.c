#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define UBRRVAL 1 // 250k baud at 8 MHz

/******************************************************************************/
void uart_init() {
    
    UBRR1H = (unsigned char)(UBRRVAL >> 8);
    UBRR1L = (unsigned char)(UBRRVAL);

    UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
    UCSR1B = (1 << TXEN1);

}

/******************************************************************************/
void uart_putchar(uint8_t tx) {

    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = tx;

}