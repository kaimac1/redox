#include <stdint.h>

void matrix_init(void);
void matrix_scan(void);
void matrix_select_row(uint8_t r);
void matrix_deselect(void);

#define ROWS 5
extern uint8_t matrix[ROWS];