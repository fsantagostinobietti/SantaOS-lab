#ifndef _STDIO_H_
#define _STDIO_H_ 

#include <stdarg.h>

#define EOF (-1)

int _putchar(int ic);
#define putchar(c)  _putchar(c)

int _puts(const char* string);
#define puts(cc)  _puts(cc)

int _printf(const char* restrict format, ...);
#define printf(...) _printf(__VA_ARGS__)


#define info(...) _printf(__VA_ARGS__)
#define warning(...) _printf(__VA_ARGS__)
#define error(...) _printf(__VA_ARGS__)
#define debug(...) _printf(__VA_ARGS__)

#endif
