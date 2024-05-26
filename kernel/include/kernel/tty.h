#ifndef ARCH_I386_TTY_H
#define ARCH_I386_TTY_H

#include <stddef.h>

void terminal_initialize(void);
void terminal_clear(void);
void terminal_putchar(char character);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);

#endif
