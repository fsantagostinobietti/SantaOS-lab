/* Surely you will remove the processor conditionals and this comment
   appropriately depending on whether or not you use C++. */

 
/* Check if the compiler thinks we are targeting the wrong operating system. 
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
*/
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
 
//#include "std.h" // useful std functions
#include "tty.h"
#include "stdio.h"
#include "serial.h"
#include "tcpip.h"


#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

static uint8_t dataPkt[2048];
static int dataPktSz;

void kernel_main(void) {
	/* Initialize terminal interface */
	terminal_initialize();
 
	/* Newline support is left as an exercise. */
	info("Hello, Unikernel World!\n");
	
	info("UART model %s\n", detect_uart_model());
	
	if ( is_serial_connected() )
	    info("Serial looks connected!\n");
	
	
	IP ipPkg;
    TCP tcpPkg;
    while (true) {
        threeWayHandshake();
        
        // receive HTTP GET request
        nextTcpIpPacket(&ipPkg, &tcpPkg);
        //debug("HTTP request:\n%s\n", tcpPkg.payload);
        
        // send HTML page
        info("html response\n");
        uint8_t data[] =    "HTTP/1.0 200 OK\n"
                            "Content-Type: text/html\n"
                            "Content-Length: 51\n"
                            "\n"
                            "<html><body><p>Hi from SantaNoOS !</p><body></html>";
        buildRawTcpIpPacket( &ipPkg, &tcpPkg,                           // inputs
                             FLAG_ACK, data, sizeof(data),              // 
                             dataPkt, &dataPktSz          // output
                           );
        sendRawPacket(dataPkt, dataPktSz);
        
        // wait for ACK
        nextTcpIpPacket(&ipPkg, &tcpPkg);
            
        // closing with client
        fourWayClose(&ipPkg, &tcpPkg);
	}
	
}

