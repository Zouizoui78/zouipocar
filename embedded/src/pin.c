#include "pin.h"
#include "bit.h"

volatile uint8_t* pin_get_direction_reg(Pin pin) {
    if (pin <= B7) return &DDRB;
    else if (pin <= C7) return &DDRC;
    else return &DDRD;
}

volatile uint8_t* pin_get_data_reg(Pin pin) {
    if (pin <= B7) return &PORTB;
    else if (pin <= C7) return &PORTC;
    else return &PORTD;
}

volatile uint8_t* pin_get_input_reg(Pin pin) {
    if (pin <= B7) return &PINB;
    else if (pin <= C7) return &PINC;
    else return &PIND;
}

uint8_t pin_get_addr(Pin pin) {
    if (pin <= B7) return pin;
    else if (pin <= C7) return pin - 8;
    else return pin - 16;
}

void pin_configure(Pin pin, PinDirection direction) {
    bit_write(pin_get_direction_reg(pin), pin_get_addr(pin), direction);
}

void pin_set(Pin pin) {
    bit_set(pin_get_data_reg(pin), pin_get_addr(pin));
}

void pin_clear(Pin pin) {
    bit_clear(pin_get_data_reg(pin), pin_get_addr(pin));
}

uint8_t pin_toggle(Pin pin) {
    return bit_toggle(pin_get_data_reg(pin), pin_get_addr(pin));
}

void pin_write(Pin pin, uint8_t state) {
    bit_write(pin_get_data_reg(pin), pin_get_addr(pin), state);
}

uint8_t pin_read(Pin pin) {
    return bit_read(pin_get_input_reg(pin), pin_get_addr(pin));
}