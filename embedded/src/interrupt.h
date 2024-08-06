#ifndef INTERRUPT
#define INTERRUPT

#include <avr/interrupt.h>
#include <avr/io.h>

void int_set_timer1_prescaler(uint16_t prescaler);

// External interrupts.

void int_setup_int0(void);
void int_enable_int0(void);
void int_disable_int0(void);

void int_setup_int1(void);
void int_enable_int1(void);
void int_disable_int1(void);

void int_setup_pcint0(void);
void int_enable_pcint0(void);
void int_disable_pcint0(void);

void int_setup_pcint1(void);
void int_enable_pcint1(void);
void int_disable_pcint1(void);

void int_setup_pcint2(void);
void int_enable_pcint2(void);
void int_disable_pcint2(void);

// Timer interrupts.

// Timer 0.
void int_setup_timer0_ovf(void);
void int_enable_timer0_ovf(void);
void int_disable_timer0_ovf(void);

void int_setup_timer0_compa(void);
void int_enable_timer0_compa(void);
void int_disable_timer0_compa(void);

void int_setup_timer0_compb(void);
void int_enable_timer0_compb(void);
void int_disable_timer0_compb(void);

// Timer 1.
void int_timer1_reset_count(void);
void int_setup_timer1_ovf(void);
void int_enable_timer1_ovf(void);
void int_disable_timer1_ovf(void);

void int_setup_timer1_compa(uint16_t comp_value);
void int_enable_timer1_compa(void);
void int_disable_timer1_compa(void);

void int_setup_timer1_compb(void);
void int_enable_timer1_compb(void);
void int_disable_timer1_compb(void);

// Timer 2.
void int_setup_timer2_ovf(void);
void int_enable_timer2_ovf(void);
void int_disable_timer2_ovf(void);

void int_setup_timer2_compa(void);
void int_enable_timer2_compa(void);
void int_disable_timer2_compa(void);

void int_setup_timer2_compb(void);
void int_enable_timer2_compb(void);
void int_disable_timer2_compb(void);

// UART interrupts.

void int_enable_uart_tx(void);
void int_disable_uart_tx(void);

void int_enable_uart_reg_empty(void);
void int_disable_uart_reg_empty(void);

void int_enable_uart_rx(void);
void int_disable_uart_rx(void);

#endif // INTERRUPT