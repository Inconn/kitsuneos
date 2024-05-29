#ifndef ARCH_I386_IDT_H
#define ARCH_I386_IDT_H

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) idtr_t;

void arch_idt_init();

#endif
