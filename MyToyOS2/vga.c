/* 
   VGA 
*/
/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}
 

 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scrollup() {
    // move VGA_HEIGHT-1 lines up
    uint16_t* dst = (uint16_t*) 0xB8000;
    uint16_t* src = dst + VGA_WIDTH;
    size_t nchars = 2 * (VGA_WIDTH * ( VGA_HEIGHT - 1 ));
    memmove(src, dst, nchars);
    
    // fill bottom line with blanks 
    size_t bottom_line = VGA_WIDTH*(VGA_HEIGHT-1);
    for (size_t idx=0; idx<VGA_WIDTH ;++idx)
        dst[bottom_line + idx] = vga_entry(' ', terminal_color);
}

void terminal_putchar(char c) {
    if ( c == '\n' ) {
        ++terminal_row;
        terminal_column = 0;
    }
	else {
	    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	    ++terminal_column;
	}
	
	if (terminal_column == VGA_WIDTH) {
	    terminal_column = 0;
	    ++terminal_row;
	}
	if (terminal_row == VGA_HEIGHT) {
		terminal_scrollup();
		terminal_row = VGA_HEIGHT-1;
	}
}
 
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

char lowNibbleHex(uint8_t b) {
    b &= 0xF;
    if ( b < 0xA )
        return '0'+b;
    return 'A'+(b-0xA); 
}

void terminal_writehex(uint8_t b) {
    char s[3];
    s[2] = 0;   // end-of-string
    s[1] = lowNibbleHex(b);
    s[0] = lowNibbleHex(b>>4);
    terminal_writestring(s);
}

