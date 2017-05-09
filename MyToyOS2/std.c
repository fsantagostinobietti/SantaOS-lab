void* memset(void * s, int c, size_t count)
{
   for ( size_t i = 0; i < count; i++ )
        ((uint8_t*) s)[i] = (uint8_t)c;
    return s;
}

void* memmove(void * s, void * d, size_t count)
{
   for ( size_t i = 0; i < count; i++ )
        ((uint8_t*) d)[i] = ((uint8_t*) s)[i];
    return d;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
