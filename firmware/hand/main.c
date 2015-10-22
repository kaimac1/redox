#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "util/xprintf.h"
#include "nrf24/nrf24.h"
#include "matrix.h"
#include "uart.h"

/******************************************************************************/
#define SLEEP_TIMEOUT 300000UL
#define DEBUG 1
#define CHANNEL 2

uint8_t msg[3];
uint8_t tx_address[5] = {0xBA,0x5E,0xBA,0x5E,0x00};
uint8_t rx_address[5] = {0x11,0x23,0x58,0x13,0x00};
uint8_t matrix_prev[ROWS];
uint8_t voltage;

void get_voltage(void);
void check_voltage(void);
void enter_sleep_mode(void);

/******************************************************************************/
int main() {

    uint8_t i, change;
    uint8_t hand;
    uint32_t timeout = 0;

    uart_init();
    xdev_out(uart_putchar);

    // Determine which hand this is from PE2
    // Left is hand 0, right is hand 1
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

    get_voltage();
    check_voltage();

    // Scan the matrix and detect any changes.
    // Modified rows are sent to the receiver.
    while (1) {  
        timeout++;
        matrix_scan();

        for (i=0; i<ROWS; i++) {
            change = matrix_prev[i] ^ matrix[i];

            // If this row has changed, send the row number and its current state
            if (change) {
                if (DEBUG) xprintf("%d %08b -> %08b   %ld\r\n", i, matrix_prev[i], matrix[i], timeout);
                msg[1] = i;
                msg[2] = matrix[i];

                nrf24_send(msg);
                while (nrf24_isSending());
                timeout = 0;
            }

            matrix_prev[i] = matrix[i];
        }

        // Sleep if there has been no activity for a while
        if (timeout > SLEEP_TIMEOUT) {
            timeout = 0;
            enter_sleep_mode();
        }
    }

}

/******************************************************************************/
// Measure the battery voltage and store in [voltage].
void get_voltage(void) {

    DIDR0 = 1;
    ADMUX = (1 << ADLAR); // Left aligned
    ADCSRA = (1 << ADPS1) | (1 << ADPS2); // 125 kHz clock
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADSC); // Start conversion

    // Wait for end of conversion and reset ADIF
    while (!(ADCSRA & (1 << ADIF)));
    ADCSRA &= ~(1 << ADIF);

    voltage = ADCH;

    ADCSRA &= ~(1 << ADEN); // Disable ADC

}

/******************************************************************************/
// Check the battery voltage and flash the LED if the battery is low.
void check_voltage(void) {

    xprintf("adc = %d\r\n", voltage);

}

/******************************************************************************/
void enter_sleep_mode(void) {

    get_voltage();

    xprintf("sleeping\r\n");

    cli();
    PORTD = 0x73;           // Select all rows
    PCMSK0 = 0xFF;          // Enable all pin change interrupts
    PCIFR = (1 << PCIF0);
    PCICR = (1 << PCIE0);


    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();                  // Enable interrupts       
    sleep_mode();

    xprintf("woke up!\r\n");
    PCICR = 0;

    check_voltage();

}

/******************************************************************************/
// Pin change interrupt - wakes the MCU up from sleep mode.
ISR(PCINT0_vect) {


    matrix_deselect();

}
