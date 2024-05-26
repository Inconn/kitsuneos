#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <string.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	terminal_clear();
}

void terminal_scroll(int lines) {
	memmove(terminal_buffer, terminal_buffer + (lines * VGA_WIDTH), (VGA_WIDTH - lines) * VGA_HEIGHT);
}

void terminal_new_line(void) {
	terminal_column = 0;
	if (terminal_row >= VGA_HEIGHT - 1)
		terminal_scroll(1);
	else
		terminal_row += 1;
}

void terminal_clear(void) {
	terminal_row = 0;
	terminal_column = 0;
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

void terminal_putentryat(char character, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(character, color);
}

void terminal_putchar(char character) {
	terminal_putentryat(character, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (terminal_row + 1 == VGA_HEIGHT)
			//terminal_row;
			terminal_new_line();
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		if (data[i] == '\n')
			terminal_new_line();
		else
			terminal_putchar(data[i]);
}

void terminal_scroll_up(void) {
	for (size_t y = 0; y < VGA_HEIGHT + 1; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			const size_t below_index = (y+1) * VGA_WIDTH + x;

			terminal_buffer[index] = terminal_buffer[below_index];
		}
	}
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
