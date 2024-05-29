#include "paging.h"

#include <string.h>

extern uint32_t _kernel_page_directory;

page_directory_entry_t* kernel_page_directory;

void arch_paging_init() {
	kernel_page_directory =  (page_directory_entry_t*)(&_kernel_page_directory + 0xC0000000);
}

void paging_invalidate_tlb() {
	asm volatile(
			"movl %cr3, %ecx;"
			"movl %ecx, %cr3;"
		    );
}
