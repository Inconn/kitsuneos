#include <stdint.h>

extern uint32_t kernel_end;

static void* current_ptr;

void dumb_alloc_init() {
	current_ptr = (void*)((uint32_t)&kernel_end - 0xC0000000);
}

void* dumb_allocate_physical(uint32_t size, uint32_t alignment) {
	void* ptr = (void*)((uint32_t)current_ptr + (alignment - ((uint32_t)current_ptr % alignment)));
	current_ptr = (void*)((uint32_t)ptr + size);

	return ptr;
}
