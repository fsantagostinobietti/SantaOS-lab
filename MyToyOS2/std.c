#include "std.h"

void* memset(void * s, int c, size_t count)
{
   for ( size_t i = 0; i < count; i++ )
        ((uint8_t*) s)[i] = (uint8_t)c;
    return s;
}

void* memcpy(void *d, const void *s, size_t n) {
    for ( size_t i = 0; i < n; i++ )
        ((uint8_t*) d)[i] = ((uint8_t*) s)[i];
    return d;
}

void* memmove(void * d, const void * s, size_t count)
{
   for ( size_t i = 0; i < count; i++ )
        ((uint8_t*) d)[i] = ((uint8_t*) s)[i];
    return d;
}

int bcmp(const void *s1, const void *s2, size_t n) {
    while ( n-- > 0 ) {
        if ( ((unsigned char*)s1)[n] != ((unsigned char*)s2)[n] )
            return -1; // sequences differ
    }
    return 0; // sequences are equal
}


size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}


