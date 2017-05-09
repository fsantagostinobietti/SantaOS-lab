/* Surely you will remove the processor conditionals and this comment
   appropriately depending on whether or not you use C++. */
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks we are targeting the wrong operating system. 
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
*/
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
 
#include "asm.c"
#include "std.c" // useful std functions
#include "vga.c"
#include "serial.c"



#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(void) {
	/* Initialize terminal interface */
	terminal_initialize();
 
	/* Newline support is left as an exercise. */
	terminal_writestring("Hello, kernel World!\n");
	
	terminal_writestring("UART model ");
	terminal_write_uart_model( detect_uart_model() );
	terminal_writestring(" \n");
	
	if ( is_serial_connected() )
	    terminal_writestring("Serial looks connected!\n");
	
	
	char cc[4];
	while (true) {
	    readserial4bytes(cc);
	    
	    // echo to screen
	    terminal_writehex(cc[0]); terminal_writehex(cc[1]); terminal_writehex(cc[2]); terminal_writehex(cc[3]);
	    terminal_writestring("   ["); 
	    terminal_write(cc, strlen(cc));
	    terminal_writestring("]\n");
	    
	    // echo to serial line
	    //write_serial( c );
	}
}

