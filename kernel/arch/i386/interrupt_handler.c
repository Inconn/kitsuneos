__attribute__((noreturn))
void exception_handler() {
	asm volatile("cli; hlt");
}
