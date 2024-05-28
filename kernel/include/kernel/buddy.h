#ifndef KERNEL_BUDDY_H
#define KERNEL_BUDDY_H

#include <stddef.h>
#include <stdbool.h>

struct buddy_block {
	size_t size;
	bool is_free;
};

struct buddy_allocator {
	struct buddy_block* head;
	struct buddy_block* tail;
	size_t alignment;
};

int buddy_initialize_allocator(struct buddy_allocator* buddy_allocator, void* data, size_t size, size_t alignment);
void* buddy_allocate(struct buddy_allocator* buddy_allocator, size_t size);
void buddy_allocator_free(struct buddy_allocator* buddy_allocator, void* data);

#endif
