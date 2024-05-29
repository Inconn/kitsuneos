#include <kernel/context_switching.h>

__attribute__((noreturn))
extern void _jump_usermode(void* user_fn_ptr);

__attribute__((noreturn))
void switch_context(void* user_fn_ptr) {
	_jump_usermode(user_fn_ptr);
}

void switch_to_user_mode() {
	asm volatile("\
			cli;\
			mov $(4 * 8) | 3, %ax; \
			mov %ax, %ds; \
			mov %ax, %es; \
			mov %ax, %fs; \
			mov %ax, %gs; \
			\
			mov %esp, %eax; \
			pushl $(4 * 8) | 3; \
			pushl %eax; \
			pushf; \
			pop %eax; \
			or $0x200, %eax; \
			push %eax; \
			pushl $(3 * 8) | 3; \
			push $1f; \
			iret; \
		1: \
			");
}
