#include "nrf24/nrf24.h"
#include "../common.h"
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

uint8_t tx_address[nrf24_ADDR_LEN] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[nrf24_ADDR_LEN] = RECEIVER_ADDRESS;
uint8_t msg[3];

void timer_init(void) {
    // 8M/64 = 
    TCNT1 = 0;
    TCCR1A = 0x00;
    TCCR1B = 0x04;
}
uint16_t timer_value(void) {
    return TCNT1;
}


int main() {

    CPU_PRESCALE(CPU_8MHz);
    timer_init();

    uart_init();
    xdev_out(uart_putchar);

    xprintf("\r\nUSB init:\t");
    usb_init();
    while (!usb_configured());
    xprintf("ok\r\n");

    xprintf("Radio init:\t");
    nrf24_init();
    nrf24_config(RF_CHANNEL, sizeof msg);
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
    uint8_t st = nrf24_getStatus();
    xprintf("done, status=%d\r\n", st);

    DDRD |= (1<<6);

    while (1) {    
        if (nrf24_dataReady()) {
            PORTD |= (1<<6);
            nrf24_getData(msg);
            //xprintf("%u\tgot %d %d %d\r\n", timer_value(), msg[0], msg[1], msg[2]);
            handle_row(msg[0], msg[1], msg[2]);
            PORTD &= ~(1<<6);
        }
    }

}
