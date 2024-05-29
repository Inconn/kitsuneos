#include <kernel/arch.h>
#include <kernel/tty.h>
#include <kernel/buddy.h>
#include <kernel/context_switching.h>
#include <kernel/test_user_fn.h>

#include <stdint.h>
#include <stdlib.h>

extern uint32_t kernel_end;

void kernel_main(uint32_t mboot_magic, void* mboot_header) {
	terminal_initialize();

	//struct buddy_allocator* b = (struct buddy_allocator*)kernel_end;

	void* data = (void*)(kernel_end + (4096 - (kernel_end % 4096)));

/*	if (buddy_initialize_allocator(b, data, 1073741824, 4096) != 0) {
		terminal_writestring("couldn't init buddy allocator\n");
		abort();
	}*/
	
	arch_init_kernel();
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
