#include <kernel/arch.h>
#include "pic.h"
#include "gdt.h"
#include "idt.h"
#include "paging.h"

void arch_init_kernel() {
	arch_paging_init();

	arch_pic_disable();

	arch_gdt_init();
	arch_idt_init();
}
