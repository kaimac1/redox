#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "util/xprintf.h"
#include "nrf24/nrf24.h"
#include "matrix.h"
#include "uart.h"

/******************************************************************************/
#define CHANNEL 2
uint8_t msg[3];
uint8_t tx_address[5] = {0xBA,0x5E,0xBA,0x5E,0x00};
uint8_t rx_address[5] = {0x11,0x23,0x58,0x13,0x00};

uint8_t matrix_prev[ROWS];
uint8_t hand = 0;

/******************************************************************************/
int main() {

    uint8_t i, change, st;

    // Enable 3.3 V regulator to power the NRF24
    //UHWCON |= (1<<UVREGE);

    // TODO: determine which hand from GPIO pin
    // TODO: set up ADC

    uart_init();
    xdev_out(uart_put_char);
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

    st = nrf24_getStatus();
    xprintf("status=%d\r\n", st);

    // Scan the matrix and detect any changes.
    // Modified rows are sent to the receiver.
    while (1) {
        matrix_scan();

        for (i=0; i<ROWS; i++) {
            change = matrix_prev[i] ^ matrix[i];
            if (change) {
                xprintf("%d %08b -> %08b\r\n", i, matrix_prev[i], matrix[i]);
                msg[1] = i;
                msg[2] = matrix[i];
                nrf24_send(msg);
                while (nrf24_isSending());
            }
            matrix_prev[i] = matrix[i];
        }
    }

}


    /*nrf24_send(out);
    while (nrf24_isSending());

    temp = nrf24_lastMessageStatus();
    if(temp == NRF24_TRANSMISSON_OK) {                    
        xprintf("ok\r\n");
    } else if(temp == NRF24_MESSAGE_LOST) {                    
        xprintf("failed\r\n");    
    }

    temp = nrf24_retransmissionCount();
    xprintf("retx: %d\r\n",temp);*/

    /* Optionally, go back to RX mode ... */
    //nrf24_powerUpRx();
    /* Or you might want to power down after TX */
    // nrf24_powerDown();            

