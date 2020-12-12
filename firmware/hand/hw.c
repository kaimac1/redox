#include "hw.h"
#include "util/xprintf.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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

/******************************************************************************/
// Sleep

void enter_sleep_mode(void) {

    xprintf("Sleep\r\n\r\n");

    cli();
    PORTD = 0x73;           // Select all rows
    PCMSK0 = 0xFF;          // Enable all pin change interrupts
    PCIFR = (1 << PCIF0);
    PCICR = (1 << PCIE0);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sei();                  // Enable interrupts
    sleep_mode();
}

// Pin change interrupt - wakes the MCU up from sleep mode.
ISR(PCINT0_vect) {

    PCICR = 0;  // Turn off pin change interrupt
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

