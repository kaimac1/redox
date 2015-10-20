#include "matrix.h"
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

uint8_t matrix[ROWS];
uint8_t row_pin[ROWS] = {0,1,4,5,6};

/******************************************************************************/
void matrix_init(void) {
	DDRB = 0;
	DDRD |= 0x73;
}

/******************************************************************************/
void matrix_scan(void) {

    uint8_t i;

    for (i=0; i<ROWS; i++) {
        matrix_select_row(i);
        _delay_us(30);
        matrix[i] = PINB;
        matrix_deselect();
    }

}

/******************************************************************************/
// Select the given row
void matrix_select_row(uint8_t r) {
    PORTD |= (1 << row_pin[r]);
}

/******************************************************************************/
// Deselect all rows
void matrix_deselect(void) {
    PORTD &= 0x8C;
}