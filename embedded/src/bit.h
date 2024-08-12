#ifndef BIT
#define BIT

#include <stdint.h>

void bit_set(volatile uint8_t *reg, uint8_t bit);
void bit_clear(volatile uint8_t *reg, uint8_t bit);
uint8_t bit_toggle(volatile uint8_t *reg, uint8_t bit);
void bit_write(volatile uint8_t *reg, uint8_t bit, uint8_t state);
uint8_t bit_read(volatile uint8_t *reg, uint8_t bit);

#endif