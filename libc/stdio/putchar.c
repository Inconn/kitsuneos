#include <stdio.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#endif

int putchar(int int_char) {
#if defined(__is_libk)
	char character = (char) int_char;
	terminal_write(&character, sizeof(character));
#else

#endif
	return int_char;
}
