#include "hw.h"
#include "matrix.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "util/xprintf.h"
#include "nrf24/nrf24.h"

#define SLEEP_TIMEOUT (100000UL) //(300000UL * 1) // 10 minutes
#define DEBUG

uint8_t tx_address[nrf24_ADDR_LEN] = RECEIVER_ADDRESS;
uint8_t rx_address[nrf24_ADDR_LEN] = HAND_ADDRESS;
uint8_t matrix_prev[NUM_ROWS];

void enter_sleep_mode(void);


void send_message(Message* msg) {

    nrf24_send((uint8_t *)msg);
    while (nrf24_isSending());
    #ifdef DEBUG
        xprintf("\terr=%d retx=%d\r\n", nrf24_lastMessageStatus(), nrf24_retransmissionCount());
    #endif
}

int main(void) {

    Message msg;
    uint32_t timeout = 0;

    hw_init();
    uint8_t hand = detect_hand();

    xprintf("\r\nHand\t%d\r\n", hand);
   
    // Initialise radio
    // Set the last byte of the address to the hand ID
    rx_address[4] = hand;
    nrf24_init();
    nrf24_config(RF_CHANNEL, sizeof(Message));
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);

    matrix_init();

    led_set(1);
    _delay_ms(250);
    led_set(0);

    msg.hand = hand;

    // Scan the matrix and detect any changes.
    // Modified rows are sent to the receiver.
    while (1) {  
        timeout++;
        matrix_scan();

        for (uint8_t i=0; i<NUM_ROWS; i++) {
            uint8_t change = matrix_prev[i] ^ matrix[i];

            // If this row has changed, send the row number and its current state
            if (change) {
                #ifdef DEBUG
                  xprintf("Row %d: %08b   %ld", i, matrix[i], timeout);
                #endif
                msg.row = i;
                msg.cols = matrix[i];

                send_message(&msg);
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

void enter_sleep_mode(void) {

    xprintf("sleeping\r\n\r\n");

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

    PCICR = 0;
    matrix_deselect();
}
