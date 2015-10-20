#include <avr/io.h>

#define set_bit(reg,bit) reg |= (1<<bit)
#define clr_bit(reg,bit) reg &= ~(1<<bit)
#define check_bit(reg,bit) (reg&(1<<bit))

void nrf24_setupPins() {
    set_bit(DDRB,7); // CE output
    set_bit(DDRB,0); // CSN output
    set_bit(DDRB,1); // SCK output
    set_bit(DDRB,2); // MOSI output
    clr_bit(DDRB,3); // MISO input
}

void nrf24_ce_digitalWrite(uint8_t state) {
    if(state) {
        set_bit(PORTB,7);
    } else {
        clr_bit(PORTB,7);
    }
}

void nrf24_csn_digitalWrite(uint8_t state) {
    if(state) {
        set_bit(PORTB,0);
    } else {
        clr_bit(PORTB,0);
    }
}

void nrf24_sck_digitalWrite(uint8_t state) {
    if(state) {
        set_bit(PORTB,1);
    } else {
        clr_bit(PORTB,1);
    }
}

void nrf24_mosi_digitalWrite(uint8_t state) {
    if(state) {
        set_bit(PORTB,2);
    } else {
        clr_bit(PORTB,2);
    }
}

uint8_t nrf24_miso_digitalRead() {
    return check_bit(PINB,3);
}
