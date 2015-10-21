#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "util/xprintf.h"
#include "nrf24/nrf24.h"
#include "matrix.h"
#include "uart.h"

/******************************************************************************/
#define DEBUG 1
#define CHANNEL 2
uint8_t msg[3];
uint8_t tx_address[5] = {0xBA,0x5E,0xBA,0x5E,0x00};
uint8_t rx_address[5] = {0x11,0x23,0x58,0x13,0x00};
uint8_t matrix_prev[ROWS];

uint8_t battery_voltage(void);

/******************************************************************************/
int main() {

    uint8_t i, change;
    uint8_t hand;

    uart_init();
    xdev_out(uart_putchar);

    // Determine which hand from PE2
    PORTE = (1 << 2);
    DDRE = 0;
    hand = (PINE & 0x04) ? 0 : 1;
    xprintf("\r\nHand %d\r\n", hand);
    
    // Initialise NRF24
    // Set the last byte of the address to the hand ID
    rx_address[4] = hand;
    nrf24_init();
    nrf24_config(CHANNEL, sizeof msg);
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);

    matrix_init();

    msg[0] = hand & 0x01;
    msg[1] = 0;
    msg[2] = 0;

    // Set up LED and flash it briefly
    DDRE |= 1<<6;
    PORTE = 1<<6;
    _delay_ms(500);
    PORTE = 0;

    battery_voltage();

    // Scan the matrix and detect any changes.
    // Modified rows are sent to the receiver.
    while (1) {        
        matrix_scan();

        for (i=0; i<ROWS; i++) {
            change = matrix_prev[i] ^ matrix[i];
            if (change) {
                if (DEBUG) xprintf("%d %08b -> %08b\r\n", i, matrix_prev[i], matrix[i]);
                msg[1] = i;
                msg[2] = matrix[i];
                nrf24_send(msg);
                while (nrf24_isSending());
            }
            matrix_prev[i] = matrix[i];
        }
    }

}

uint8_t battery_voltage(void) {

    DIDR0 = 1;
    ADMUX = (1 << ADLAR); // Left aligned
    ADCSRA = (1 << ADPS1) | (1 << ADPS2); // 125 kHz clock
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADSC); // Start conversion

    // Wait for end of conversion and reset ADIF
    while (!(ADCSRA & (1 << ADIF)));
    ADCSRA &= ~(1 << ADIF);

    xprintf("%d\r\n", ADCH);

    ADCSRA &= ~(1 << ADEN);

}