/* ASM functions definition
   from http://wiki.osdev.org/Inline_Assembly/Examples */
   
#ifndef _AMS_H_
#define _AMS_H_

#include <stdint.h>

void outb(uint16_t port, uint8_t val);

uint8_t inb(uint16_t port);


#endif /* _AMS_H_ */
