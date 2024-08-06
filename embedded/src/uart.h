#ifndef UART
#define UART

#include <stdint.h>
#include <stddef.h>

typedef enum UARTRetCodes {
    UART_OK,
    UART_TIMEOUT
} UARTRetCodes;

typedef enum UARTInterruptsFlags {
    UART_TX_INTERRUPT = 0b001,
    UART_RX_INTERRUPT = 0b010,
    UART_TIMEOUT_INTERRUPT = 0b100
} UARTInterruptsFlags;

void uart_enable_tx(void);
void uart_enable_rx(void);

void uart_disable_tx(void);
void uart_disable_rx(void);

void uart_set_baudrate(uint32_t baudrate, uint32_t fosc);

void uart_init(uint32_t baudrate, uint32_t fosc, uint8_t enable_interrupts_flags);

/**
 * @brief Tells whether the tx buffer is empty and ready to receive data.
 * 
 * @return int 1 => empty and ready to receive data ; 0 => not empty
 */
int uart_tx_reg_empty(void);

/**
 * @brief Tells whether the tx buffer has sent its data.
 * 
 * @return int 1 => data sent ; 0 => data still in the buffer
 */
int uart_tx_complete(void);

/**
 * @brief Tells whether the rx buffer has received data.
 * 
 * @return int 1 => received data ; 0 => empty buffer
 */
int uart_rx_complete(void);

// Returns UART_TIMEOUT in case of timeout.
uint8_t uart_write_byte(uint8_t data);

// Returns UART_TIMEOUT in case of timeout.
uint8_t uart_write_array(uint8_t *data, size_t size);

// Returns UART_TIMEOUT in case of timeout.
uint8_t uart_write_string(char *string);

uint8_t uart_read_byte(uint16_t timeout);
uint8_t uart_read_array(uint8_t *output, size_t *output_size, uint8_t packet_delimiter, size_t packet_max_size, uint16_t timeout);
uint8_t uart_read_string(char *output, size_t *output_size, char string_delimiter, size_t string_max_size, uint16_t timeout);

void uart_flush(void);

#endif // UART