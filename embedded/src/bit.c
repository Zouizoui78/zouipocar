#include "bit.h"
#include <avr/io.h>

void bit_set(volatile uint8_t * reg, uint8_t bit) {
    *reg |= _BV(bit);
}

void bit_clear(volatile uint8_t * reg, uint8_t bit) {
    *reg &= ~_BV(bit);
}

void bit_write(volatile uint8_t * reg, uint8_t bit, uint8_t state) {
    if (state == 0) bit_clear(reg, bit);
    else bit_set(reg, bit);
}

uint8_t bit_read(volatile uint8_t * reg, uint8_t bit) {
    return (*reg & _BV(bit)) >> bit;
}

uint8_t bit_toggle(volatile uint8_t * reg, uint8_t bit) {
    *reg ^= _BV(bit);
    return bit_read(reg, _BV(bit));
}