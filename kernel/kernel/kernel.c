#include <kernel/tty.h>
#include <kernel/buddy.h>

void kernel_main(void) {
	terminal_initialize();
	initialize_buddy_allocator();
	terminal_writestring("welcome to kitsuneOS!\nthis is all it does right now...\n");

	char* allocated = (char*)buddy_allocate(40);

	allocated = "this is a buddy allocated string\n\0";

	terminal_writestring(allocated);
	terminal_writestring("wow!\n");
}
