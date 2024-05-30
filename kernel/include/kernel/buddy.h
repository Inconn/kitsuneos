#ifndef KERNEL_BUDDY_H
#define KERNEL_BUDDY_H

#include <stdbool.h>

#define BUDDY_MIN_ORDER 12
#define BUDDY_MAX_ORDER 19

struct double_linked_list {
	struct double_linked_list* prev;
	struct double_linked_list* next;
	void* data;
}

struct buddy_block_metadata {
	bool is_free;
	uint8_t order;
}
typedef struct buddy_metadata_block buddy_block_metadata_t;

struct buddy_allocator {
	buddy_block_metadata_t* physical_page_metadata;
	void* memory_block;
	struct double_linked_list** freelists;
	uint32_t size;
}
typedef struct buddy_allocator buddy_allocator_t;

int buddy_init_allocator(buddy_allocator_t* allocator, buddy_block_metadata_t* metadata_block, void* memory_block, uint32_t physical_page_count);

#endif
