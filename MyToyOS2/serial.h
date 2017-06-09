#ifndef _SERIAL_H_
#define _SERIAL_H_

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif

enum uart_model {
    UART_16750  = 0,
    UART_16550A = 1,
    UART_16450  = 2,
    UART_8250   = 3
};

enum uart_model detect_uart_model();

void terminal_write_uart_model(enum uart_model model);

char read_serial();

void write_serial(char a);

bool is_serial_connected();

void readserial4bytes(char arr[]);

#endif /* _SERIAL_H_ */
