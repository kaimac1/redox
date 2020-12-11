#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "util/xprintf.h"
#include "nrf24/nrf24.h"
#include "matrix.h"
#include "uart.h"

const uint8_t channel = 20;
uint8_t tx_address[5] = {0x4E,0xD0,0xBA,0x5E,0x00};
uint8_t rx_address[5] = {0x4E,0xD0,0x00,0x00,0x00};

/******************************************************************************/
#define SLEEP_TIMEOUT (100000UL) //(300000UL * 1) // 10 minutes
#define DEBUG

uint8_t msg[3];
uint8_t matrix_prev[ROWS];
uint8_t voltage;

void get_voltage(void);
void check_voltage(void);
void enter_sleep_mode(void);

void send_msg(uint8_t* msg) {

    nrf24_send(msg);
    while (nrf24_isSending());
    #ifdef DEBUG
        xprintf("\terr=%d retx=%d\r\n", nrf24_lastMessageStatus(), nrf24_retransmissionCount());
    #endif

}

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
    xprintf("\r\nHand\t%d\r\n", hand);
    xprintf("Channel\t%d\r\n", channel);
    
    // Initialise NRF24
    // Set the last byte of the address to the hand ID
    rx_address[4] = hand;
    nrf24_init();
    nrf24_config(channel, sizeof msg);
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
                #ifdef DEBUG
                  xprintf("Row %d: %08b   %ld", i, matrix[i], timeout);
                #endif
                msg[1] = i;
                msg[2] = matrix[i];

                send_msg(msg);
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

/******************************************************************************/
// Check the battery voltage and flash the LED if the battery is low.
void check_voltage(void) {

    // TODO: finish this
    if (voltage < 76) {
        PORTE = 1<<6;
        _delay_ms(1500);
        PORTE = 0;
    }

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


    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sei();                  // Enable interrupts       
    sleep_mode();

    check_voltage();

}

/******************************************************************************/
// Pin change interrupt - wakes the MCU up from sleep mode.
ISR(PCINT0_vect) {

    PCICR = 0;
    matrix_deselect();

}
