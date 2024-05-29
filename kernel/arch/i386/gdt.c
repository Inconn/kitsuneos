#include <stdint.h>

#include "gdt.h"
#include "tss.h"


uint64_t gdt[6] __attribute__((aligned(8))) = { 0 };

static gdtr_t gdtr;

static uint32_t gdt_size = sizeof(gdt) - 1;

static tss_entry_t tss;

extern void tss_flush();

int set_gdt_entry(uint8_t* gdt_entry, uint32_t base, uint32_t limit, uint8_t access_byte, uint8_t flags) {
	if (limit > 0xFFFFF)
		return -1;

	gdt_entry[0] = limit & 0xFF;
	gdt_entry[1] = (limit >> 8) & 0xFF;
	gdt_entry[6] = (limit >> 16) & 0x0F;

	gdt_entry[2] = base & 0xFF;
	gdt_entry[3] = (base >> 8) & 0xFF;
	gdt_entry[4] = (base >> 16) & 0xFF;
	gdt_entry[7] = (base >> 24) & 0xFF;

	gdt_entry[5] = access_byte;

	gdt_entry[6] |= (flags << 4);

	return 0;
}

void arch_gdt_init() {
	gdtr.base = (uintptr_t)&gdt[0];
	gdtr.limit = (uint16_t)sizeof(uint64_t) * 6 - 1;

	asm volatile("lgdt %0" : : "m"(gdtr));


	set_gdt_entry((uint8_t*)&gdt[1], 0, 0xFFFFF, 0x9A, 0xC);
	set_gdt_entry((uint8_t*)&gdt[2], 0, 0xFFFFF, 0x92, 0xC);
	set_gdt_entry((uint8_t*)&gdt[3], 0, 0xFFFFF, 0xFA, 0xC);
	set_gdt_entry((uint8_t*)&gdt[4], 0, 0xFFFFF, 0xF2, 0xC);
	arch_tss_write((gdt_entry_t*)&gdt[5], &tss, sizeof(tss) - 1);

	tss_flush();
}
