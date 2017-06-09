#ifndef _STD_H_
#define _STD_H_

#include <stddef.h>  // std types
#include <stdint.h>  // int types

void* memset(void * s, int c, size_t count);

void* memcpy(void *d, const void *s, size_t n);

void* memmove(void * d, const void * s, size_t count);

int bcmp(const void *s1, const void *s2, size_t n);

size_t strlen(const char* str);

#endif /* _STD_H_ */
