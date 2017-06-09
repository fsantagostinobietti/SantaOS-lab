#ifndef _TTY_H_
#define _TTY_H_

#include <stddef.h>  // std types
#include <stdint.h>  // int types

void terminal_initialize(void);

void terminal_write(const char* data, size_t size);

void terminal_writestring(const char* data);

void terminal_writehex(uint8_t b);

#endif /* _TTY_H_ */
