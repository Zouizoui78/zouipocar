#include "interrupt.h"
#include "bit.h"
#include "constants.h"

void int_set_timer1_prescaler(uint16_t prescaler) {
    switch (prescaler)
    {
    case 1:
        bit_set(&TCCR1B, CS10);
        break;
    case 8:
        bit_set(&TCCR1B, CS11);
        break;
    case 64:
        bit_set(&TCCR1B, CS10);
        bit_set(&TCCR1B, CS11);
        break;
    case 256:
        bit_set(&TCCR1B, CS12);
        break;
    case 1024:
        bit_set(&TCCR1B, CS10);
        bit_set(&TCCR1B, CS12);
        break;
    default:
        break;
    }
}

// UART interrupts.

void int_enable_uart_tx(void) {
    bit_set(&UCSR0B, TXCIE0);
}

void int_disable_uart_tx(void) {
    bit_clear(&UCSR0B, TXCIE0);
}

void int_enable_uart_reg_empty(void) {
    bit_set(&UCSR0B, UDRIE0);
}

void int_disable_uart_reg_empty(void) {
    bit_clear(&UCSR0B, UDRIE0);
}

void int_enable_uart_rx(void) {
    bit_set(&UCSR0B, RXCIE0);
}

void int_disable_uart_rx(void) {
    bit_clear(&UCSR0B, RXCIE0);
}

void int_timer1_reset_count(void) {
    TCNT1 = 0;
}

void int_setup_timer1_compa(uint16_t comp_value) {
    bit_set(&TCCR1B, WGM12); // Clear timer on compare match.

    // Write compare match value.
    OCR1A = comp_value;
}

void int_enable_timer1_compa(void) {
    bit_set(&TIMSK1, OCIE1A);
}

void int_disable_timer1_compa(void) {
    bit_clear(&TIMSK1, OCIE1A);
}