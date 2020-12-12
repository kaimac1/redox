#include "hw.h"
#include "matrix.h"
#include "nrf24/nrf24.h"
#include "util/xprintf.h"
#include <stdint.h>
#include <util/delay.h>

#define IDLE_SLEEP_TICKS (10000UL)
#define DEBUG

uint8_t tx_address[nrf24_ADDR_LEN] = RECEIVER_ADDRESS;
uint8_t rx_address[nrf24_ADDR_LEN] = HAND_ADDRESS;
uint8_t matrix_prev[NUM_ROWS];


void send_message(Message* msg) {

    nrf24_send((uint8_t *)msg);
    while (nrf24_isSending());
    #ifdef DEBUG
        xprintf("\terr=%d retx=%d\r\n", nrf24_lastMessageStatus(), nrf24_retransmissionCount());
    #endif
    nrf24_powerDown();
}

int main(void) {

    Message msg;
    uint32_t timeout = 0;

    hw_init();
    uint8_t hand = detect_hand();

    xprintf("\r\nHand %d\r\n", hand);
   
    // Initialise radio
    // Set the last byte of the address to the hand ID
    rx_address[4] = hand;
    nrf24_init();
    nrf24_config(RF_CHANNEL, sizeof(Message));
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
    nrf24_powerDown();

    matrix_init();

    led_set(1);
    _delay_ms(250);
    led_set(0);

    msg.hand = hand;

    // Scan the matrix and detect any changes.
    // Modified rows are sent to the receiver.
    while (1) {  
        matrix_scan();

        uint8_t keys_down = 0;

        for (uint8_t i=0; i<NUM_ROWS; i++) {
            uint8_t change = matrix_prev[i] ^ matrix[i];
            keys_down |= matrix[i];

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

        // Sleep after no keys have been held down for a while.
        if (!keys_down) {
            timeout++;
        } else {
            timeout = 0;
        }
        if (timeout > IDLE_SLEEP_TICKS) {
            timeout = 0;
            enter_sleep_mode();
        }
    }
}

