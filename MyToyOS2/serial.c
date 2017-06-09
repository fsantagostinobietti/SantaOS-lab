/* 
   serial I/O 
*/

#include "serial.h"
#include "asm.h"
#include "tty.h"

#define PORT 0x3F8   /* COM1 */



void terminal_write_uart_model(enum uart_model model) {
    switch(model) {
        case UART_16750:
            terminal_writestring("16750");
            break;
        case UART_16550A:
            terminal_writestring("16550A");
            break;
        case UART_16450:
            terminal_writestring("16450");
            break;
        case UART_8250:
            terminal_writestring("8250");
            break;
        default:
            terminal_writestring("n/a");
    }
}

enum uart_model detect_uart_model() {
    outb(PORT+2, 0xE7); // FIFO Control Register
    uint8_t b = inb(PORT+2);   // Interrupt Identification Register
    if ( b & 0x40 )
        if ( b & 0x80 ) 
            if ( b & 0x20 )
                return UART_16750;
            else
                return UART_16550A;
        else
            return UART_16750;
    else  {
        outb(PORT+7, 0x2A); // Scratch Register
        if ( inb(PORT+7) == 0x2A )
            return UART_16450;
        else 
            return UART_8250;
    }
}

int serial_received() {
   return inb(PORT + 5) & 1;
}
 
char read_serial() {
   while (serial_received() == 0);
 
   return inb(PORT);
}

int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}
 
void write_serial(char a) {
   while (is_transmit_empty() == 0);
 
   outb(PORT, a);
}

bool is_serial_connected() {
    // write in scratch register
    outb(PORT + 7, 0xba);
    // read from scratch register
    uint8_t b = inb(PORT + 7);
    return (b == 0xba);
}

void readserial4bytes(char arr[]) {
    arr[0] = read_serial();
    arr[1] = read_serial();
    arr[2] = read_serial();
    arr[3] = read_serial();
}

