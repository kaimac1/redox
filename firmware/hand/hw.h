#pragma once
#include <stdint.h>
#include "../common.h"

#define set_bit(reg,bit)    (reg) |= (1<<(bit))
#define clr_bit(reg,bit)    (reg) &= ~(1<<(bit))
#define check_bit(reg,bit) ((reg) & (1<<(bit)))

void hw_init(void);
uint8_t detect_hand(void);

void uart_init(void);
void uart_putchar(uint8_t tx);

void led_init(void);
void led_set(uint8_t set);
