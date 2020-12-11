#include <stdint.h>
#include "../common.h"

void matrix_init(void);
void matrix_scan(void);
void matrix_select_row(uint8_t r);
void matrix_deselect(void);

extern uint8_t matrix[NUM_ROWS];