#include "nrf24/nrf24.h"
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "util/xprintf.h"
#include "uart.h"
#include "usb.h"
#include "keyboard.h"

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00
#define CPU_8MHz        0x01

/******************************************************************************/
#define CHANNEL 2
uint8_t msg[3];
uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xBA,0x5E,0xBA,0x5E,0x00};

/******************************************************************************/
int main() {

    CPU_PRESCALE(CPU_8MHz);

    uart_init();
    xdev_out(uart_putchar);

    xprintf("\r\nInitialising USB...\r\n");
    usb_init();
    while (!usb_configured());
    _delay_ms(1000);

    nrf24_init();
    nrf24_config(CHANNEL, sizeof msg);
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);

    xprintf("Ready\r\n");
    uint8_t st = nrf24_getStatus();
    xprintf("status=%d\r\n", st);

    DDRD |= (1<<6);

    while (1) {    
        if (nrf24_dataReady()) {
            PORTD |= (1<<6);
            nrf24_getData(msg);
            xprintf("got %d %d %d\r\n", msg[0], msg[1], msg[2]);
            handle_row(msg[0], msg[1], msg[2]);
            PORTD &= ~(1<<6);
        }
    }

}
