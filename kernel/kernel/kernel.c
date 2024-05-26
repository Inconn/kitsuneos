#include <kernel/tty.h>

void kernel_main(void) {
	terminal_initialize();
	terminal_writestring("welcome to kitsuneOS!\nthis is all it does right now...");
}
