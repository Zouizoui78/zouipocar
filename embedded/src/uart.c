#include "uart.h"
#include "bit.h"
#include "interrupt.h"
#include "return_codes.h"

#include <math.h>
#include <string.h>

#include <avr/io.h>

#define WRITE_TIMEOUT 5
volatile uint16_t uart_timeout_counter = 0;

ISR(TIMER1_COMPA_vect) {
    ++uart_timeout_counter;
}

void reset_timeout_counter(void) {
    uart_timeout_counter = 0;
    int_timer1_reset_count();
}

void uart_enable_tx(void) {
    bit_set(&UCSR0B, TXEN0);
}

void uart_enable_rx(void) {
    bit_set(&UCSR0B, RXEN0);
}

void uart_disable_tx(void) {
    bit_clear(&UCSR0B, TXEN0);
}

void uart_disable_rx(void) {
    bit_clear(&UCSR0B, RXEN0);
}

void uart_set_baudrate(uint32_t baudrate, uint32_t fosc) {
    double res = fosc / 16.0 / baudrate - 1;
    if (res < 0) UBRR0 = 0;
    else UBRR0 = (int)round(res);
}

void uart_init(uint32_t baudrate, uint32_t fosc, uint8_t enable_interrupts_flags) {
    uart_set_baudrate(baudrate, fosc);
    uart_enable_tx();
    uart_enable_rx();

    if ((enable_interrupts_flags & UART_TX_INTERRUPT) != 0) {
        int_enable_uart_tx();
    }

    if ((enable_interrupts_flags & UART_RX_INTERRUPT) != 0) {
        int_enable_uart_rx();
    }

    if ((enable_interrupts_flags & UART_TIMEOUT_INTERRUPT) != 0) {
        // We want to run timer1 compa interrupt every second
        // to increment a counter used for timeouts.
        // prescaler = 256 is enough at 16MHz to get an interrupt
        // every second since 16e6 / 256 = 62500 and timer1 has 16 bits.
        int_set_timer1_prescaler(256);
        int_setup_timer1_compa(62500);
        int_enable_timer1_compa();
    }
}

int uart_tx_reg_empty(void) {
    return bit_read(&UCSR0A, UDRE0);
}

int uart_tx_complete(void) {
    return bit_read(&UCSR0A, TXC0);
}

int uart_rx_complete(void) {
    return bit_read(&UCSR0A, RXC0);
}

uint8_t uart_write_byte(uint8_t data) {
    reset_timeout_counter();

    // Wait for uart tx buffer to be empty.
    while (uart_timeout_counter < WRITE_TIMEOUT) {
        if (uart_tx_reg_empty() == 1) {
            // Not empty, ready to send.
            break;
        }
    }

    // If not empty then we timed out.
    if (uart_tx_reg_empty() == 0)
        return UART_TIMEOUT;

    // Write data in tx buffer.
    UDR0 = data;

    // Then wait for data to be sent.
    reset_timeout_counter();
    while (uart_timeout_counter < WRITE_TIMEOUT) {
        if (uart_tx_complete() == 1) {
            return UART_OK;
        }
    }

    // If we get here, we timed out.
    return UART_TIMEOUT;
}

uint8_t uart_write_array(uint8_t *data, size_t size) {
    for (uint8_t i = 0 ; i < size ; ++i) {
        if (uart_write_byte(data[i]) == UART_TIMEOUT) {
            return UART_TIMEOUT;
        }
    }
    return UART_OK;
}

uint8_t uart_write_string(char *string) {
    return uart_write_array((uint8_t*)string, strlen(string));
}

uint8_t uart_read_byte(uint16_t timeout) {
    reset_timeout_counter();
    while (uart_timeout_counter < timeout) {
        if (uart_rx_complete() == 1) {
            return UDR0;
        }
    }

    // Return 0 on timeout.
    return 0;
}

uint8_t uart_read_array(uint8_t *output, size_t *output_size, uint8_t packet_delimiter, size_t packet_max_size, uint16_t timeout) {
    while (*output_size < packet_max_size) {
        uint8_t byte = uart_read_byte(timeout);
        if (byte == 0) // timeout
            return UART_TIMEOUT;
        output[*output_size] = byte;
        ++*output_size;
        if (output[*output_size - 1] == packet_delimiter) break;
    }
    return UART_OK;
}

uint8_t uart_read_string(char *output, size_t *output_size, char string_delimiter, size_t string_max_size, uint16_t timeout) {
    uint8_t ret = uart_read_array((uint8_t*)output, output_size, string_delimiter, string_max_size, timeout);

    if (ret == UART_TIMEOUT)
        return UART_TIMEOUT;

    output[*output_size] = 0;

    return UART_OK;
}

void uart_flush(void) {
    uint8_t tmp;
    while (uart_rx_complete() == 1)
        tmp = UDR0;
    (void)tmp;
}