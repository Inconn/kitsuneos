#include "gdt.h"
#include "tss.h"

#include <string.h>
#include <stdint.h>

extern uint32_t stack_top;
extern void tss_flush();

void arch_tss_write(gdt_entry_t* gdt_entry, tss_entry_t* tss_entry, uint32_t tss_entry_size) {
	uint32_t base = (uintptr_t)tss_entry;
	uint32_t limit = tss_entry_size;

	gdt_entry->limit_low = limit;
	gdt_entry->base_low = base;
	gdt_entry->accessed = 1;
	gdt_entry->read_write = 0;
	gdt_entry->conforming_expand_down = 0;
	gdt_entry->code = 1;
	gdt_entry->code_data_segment = 0;
	gdt_entry->DPL = 0;
	gdt_entry->present = 1;
	gdt_entry->limit_high = (limit & (0xF << 16)) >> 16;
	gdt_entry->available = 0;
	gdt_entry->long_mode = 0;
	gdt_entry->big = 0;
	gdt_entry->gran = 0;
	gdt_entry->base_high = (base & (0xFF << 24)) >> 24;

	memset(tss_entry, 0, tss_entry_size);

	tss_entry->ss0 = (2 * 8);
	tss_entry->esp0 = stack_top;
}

void set_kernel_stack(uint32_t stack, tss_entry_t* tss_entry) {
	tss_entry->esp0 = stack;
}
