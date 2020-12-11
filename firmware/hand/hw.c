#include "hw.h"
#include "util/xprintf.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

void hw_init(void) {

    uart_init();
    led_init();

}

uint8_t detect_hand(void) {
    // Hacky because this was forgotten about
    set_bit(PORTE, 2);
    clr_bit(DDRE, 2);
    return check_bit(PINE, 2) ? HAND_LEFT : HAND_RIGHT;    
}

/******************************************************************************/
// UART

#define UBRRVAL 1 // 250k baud at 8 MHz

void uart_init(void) {
    
    UBRR1 = UBRRVAL;
    UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
    UCSR1B = (1 << TXEN1);

    xdev_out(uart_putchar);

}

void uart_putchar(uint8_t tx) {

    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = tx;

}

/******************************************************************************/
// LED

void led_init(void) {
    // PE6
    set_bit(DDRE, 6);
}

void led_set(uint8_t set) {
    if (set) {
        set_bit(PORTE, 6);
    } else {
        clr_bit(PORTE, 6);
    }
}



// Battery

/*
// Measure the battery voltage and store in [voltage].
#define ADC_N 10
void get_voltage(void) {

    uint8_t i;
    uint16_t v = 0;

    DIDR0 = 1;
    ADMUX = (1 << ADLAR); // Left aligned
    ADCSRA = (1 << ADPS1) | (1 << ADPS2); // 125 kHz clock
    ADCSRA |= (1 << ADEN);
    
    // Get an average of 10 readings
    for (i=0; i<ADC_N; i++) {
        ADCSRA |= (1 << ADSC); // Start conversion
        while (!(ADCSRA & (1 << ADIF)));
        ADCSRA &= ~(1 << ADIF);

        v += ADCH;
    }

    voltage = v / ADC_N;    

    ADCSRA &= ~(1 << ADEN); // Disable ADC


}

// Check the battery voltage and flash the LED if the battery is low.
void check_voltage(void) {

    // TODO: finish this
    if (voltage < 76) {
        PORTE = 1<<6;
        _delay_ms(1500);
        PORTE = 0;
    }

}
*/

