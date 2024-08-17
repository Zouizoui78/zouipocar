#ifndef PIN
#define PIN

#include <avr/io.h>

typedef enum Pin {
    B0,
    B1,
    B2,
    B3,
    B4,
    B5,
    B6,
    B7,
    C0,
    C1,
    C2,
    C3,
    C4,
    C5,
    C6,
    C7,
    D0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7
} Pin;

typedef enum PinDirection { INPUT, OUTPUT } PinDirection;

volatile uint8_t *pin_get_direction_reg(Pin pin);
volatile uint8_t *pin_get_data_reg(Pin pin);
volatile uint8_t *pin_get_input_reg(Pin pin);
uint8_t pin_get_addr(Pin pin);

void pin_configure(Pin pin, PinDirection direction);

void pin_set(Pin pin);
void pin_clear(Pin pin);
uint8_t pin_toggle(Pin pin);
void pin_write(Pin pin, uint8_t state);
uint8_t pin_read(Pin pin);

#endif