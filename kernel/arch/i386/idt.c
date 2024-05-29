#include <stdint.h>

#include "idt.h"

//uint32_t idtr[2] = { 0 };


typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

static idt_entry_t idt[256] __attribute__((aligned(8))) = { 0 };

static idtr_t idtr;

extern uint32_t isr_stub();

void set_idt_entry(uint8_t vector, void* isr, uint8_t flags) {
	idt_entry_t* descriptor = &idt[vector];

	descriptor->isr_low = (uint32_t)isr & 0xFFFF;
	descriptor->kernel_cs = 0x08;
	descriptor->attributes = flags;
	descriptor->isr_high = (uint32_t)isr >> 16;
	descriptor->reserved = 0;
}

void arch_idt_init() {
	for (uint16_t vector = 0; vector < 256; vector++) {
		if (vector != 15)
			set_idt_entry(vector, isr_stub, 0x8E);
	}

	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;


	asm volatile("lidt %0" : : "m"(idtr));
	asm volatile("sti");
}
