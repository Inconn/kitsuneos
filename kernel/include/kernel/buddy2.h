#ifndef KERNEL_BUDDY_H
#define KERNEL_BUDDY_H

#include <stdbool.h>
#include <stdint.h>

#define BUDDY_MIN_ORDER 12
#define BUDDY_MAX_ORDER 19

struct buddy_allocator {
	uint32_t size;
	uint32_t** bitmaps;
	void* memory_block;
};
typedef struct buddy_allocator buddy_allocator_t;

int buddy_init_allocator(buddy_allocator_t* allocator, void* bookkeeping_block, uint32_t bookkeeping_size, void* memory_block, uint32_t size);

void* buddy_allocate(buddy_allocator_t* allocator, uint32_t size);

int buddy_free(buddy_allocator_t* allocator, void* memory_to_free);

#endif
