#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define uart_pin_output()    DDRD |= (1<<3)
#define uart_set_pin()        PORTD |= (1<<3)
#define uart_clr_pin()        PORTD &= ~(1<<3)
#define uart_bit_dly()        _delay_us(100)

void uart_init() {
    uart_pin_output();
}
void uart_put_char(uint8_t tx) {
    uint8_t i;

    uart_clr_pin();
    uart_bit_dly();

    for (i=0;i<8;i++) {
        if (tx & (1<<i)) {
            uart_set_pin();
        } else {
            uart_clr_pin();
        }

        uart_bit_dly();
    }

    uart_set_pin();
    uart_bit_dly();
}