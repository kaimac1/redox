#include <avr/io.h>

#define set_bit(reg,bit) reg |= (1<<bit)
#define clr_bit(reg,bit) reg &= ~(1<<bit)
#define check_bit(reg,bit) (reg&(1<<bit))

void nrf24_setupPins() {
    set_bit(DDRF,4); // CE output
    set_bit(DDRF,5); // CSN output
    set_bit(DDRF,6); // SCK output
    set_bit(DDRF,7); // MOSI output
    clr_bit(DDRC,7); // MISO input
}

void nrf24_ce_digitalWrite(uint8_t state) {
    if(state) {
        set_bit(PORTF,4);
    } else {
        clr_bit(PORTF,4);
    }
}

void nrf24_csn_digitalWrite(uint8_t state) {
    if(state) {
        set_bit(PORTF,5);
    } else {
        clr_bit(PORTF,5);
    }
}

void nrf24_sck_digitalWrite(uint8_t state) {
    if(state) {
        set_bit(PORTF,6);
    } else {
        clr_bit(PORTF,6);
    }
}

void nrf24_mosi_digitalWrite(uint8_t state) {
    if(state) {
        set_bit(PORTF,7);
    } else {
        clr_bit(PORTF,7);
    }
}

uint8_t nrf24_miso_digitalRead() {
    return check_bit(PINC,7);
}
