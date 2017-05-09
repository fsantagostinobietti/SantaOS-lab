/* ASM functions definition
   from http://wiki.osdev.org/Inline_Assembly/Examples */
   
   
/* IN/OUT functions*/

/* The "a" modifier enforces val to be placed in the eax register before the asm command is issued 
   and "Nd" allows for one-byte constant values to be assembled as constants, freeing the edx register for other cases

   see https://gcc.gnu.org/onlinedocs/gcc/Constraints.html#Constraints
*/
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %[v], %[p]" 
                    : // no output operand
                    : [v] "a"(val), [p] "Nd"(port) 
                    : // no clobbers
                 );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %[addr], %[val]"
                   : [val] "=a"(ret)    // output operand: use 'a' register
                   : [addr] "Nd"(port)  // input operand: use 'd' register, u8 value
                   : // no clobbers
                 );
    return ret;
}

