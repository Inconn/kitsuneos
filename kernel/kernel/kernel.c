#include <kernel/arch.h>
#include <kernel/tty.h>
#include <kernel/alloc.h>
#include <kernel/context_switching.h>
#include <kernel/test_user_fn.h>

#include <stdint.h>
#include <stdlib.h>

extern uint32_t kernel_end;

void kernel_main(uint32_t mboot_magic, void* mboot_header) {
	terminal_initialize();

	arch_init_kernel();
	if (init_kalloc() != 0) {
		terminal_writestring("couldn't init kernel allocator\n");
		abort();
	}
	
	terminal_writestring("welcome to kitsuneOS!\nthis is all it does right now...\n");

	//char* allocated = (char*)buddy_allocate(b, 40);

	//allocated = "this is a buddy allocated string\n\0";

	//terminal_writestring(allocated);
	terminal_writestring("wow!\n");

	/*for (;;) {
		asm volatile("hlt;");
	}*/

	//switch_context(&test_user_fn);
}
