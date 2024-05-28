#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <kernel/buddy.h>

struct buddy_block* buddy_next_buddy(struct buddy_block* block) {
	return (struct buddy_block*)((char*)block + block->size);
}

int buddy_initialize_allocator(struct buddy_allocator* buddy_allocator, void* data, size_t size, size_t alignment) {
	if (data == NULL)
		return -1;
	if (size == 0 || (size & (size - 1)) != 0)
		return -2;
	if (alignment == 0 || (alignment & (alignment - 1)) != 0)
		return -3;

	while (alignment < sizeof(struct buddy_block)) {
		alignment <<= 1;
	}

	if ((uintptr_t)data % alignment != 0)
		return -4;

//	head = (void*)(end_kernel + (buddy_allocator->alignment - (end_kernel % buddy_allocator->alignment)));

	buddy_allocator->head = (struct buddy_block*) data;
	buddy_allocator->head->size = size;
	buddy_allocator->head->is_free = true;

	buddy_allocator->tail = buddy_next_buddy(buddy_allocator->head);

	buddy_allocator->alignment = alignment;

	return 0;
}

size_t buddy_required_size(struct buddy_allocator* buddy_allocator, size_t size) {
	size_t actual_size = buddy_allocator->alignment;

	size += sizeof(struct buddy_block);
	size += (buddy_allocator->alignment - (size % buddy_allocator->alignment));

	while (size > actual_size) {
		actual_size <<= 1;
	}
	
	return actual_size;
}


void buddy_coalescence(struct buddy_allocator* buddy_allocator) {
	for (;;) {
		struct buddy_block* block = buddy_allocator->head;
		struct buddy_block* buddy = buddy_next_buddy(block);

		bool no_coalescence = true;
		while (block < buddy_allocator->tail && buddy < buddy_allocator->tail) {
			if (block ->is_free && buddy->is_free && block->size == buddy->size) {
				block->size <<= 1;
				block = buddy_next_buddy(block);
				if (block < buddy_allocator->tail) {
					buddy = buddy_next_buddy(block);
					no_coalescence = false;
				}
			} else if (block->size < buddy->size) {
				block = buddy;
				buddy = buddy_next_buddy(buddy);
			} else {
				block = buddy_next_buddy(buddy);
				if (block < buddy_allocator->tail) {
					buddy = buddy_next_buddy(block);
				}
			}
		}

		if (no_coalescence) {
			return;
		}
	}
}

struct buddy_block* buddy_block_split(struct buddy_block* block, size_t size) {
	if (block != NULL && size != 0) {
		while (size < block->size) {
			size_t sz = block->size >> 1;
			block->size = sz;
			block = buddy_next_buddy(block);
			block->size = sz;
			block->is_free = true;
		}

		if (size <= block->size) {
			return block;
		}
	}

	return NULL;
}

struct buddy_block* buddy_find_best_block(struct buddy_allocator* buddy_allocator, size_t size) {
	struct buddy_block* best_block = NULL;
	struct buddy_block* block = buddy_allocator->head;
	struct buddy_block* buddy = buddy_next_buddy(block);

	if (buddy == buddy_allocator->tail && block->is_free) {
		return buddy_block_split(block, size);
	}

	while (block < buddy_allocator->tail && buddy < buddy_allocator->tail) {
		if (block->is_free && buddy->is_free && block->size == buddy->size) {
			block->size <<= 1;
			if (size <= block->size && (best_block == NULL || block->size <= best_block->size)) {
				best_block = block;
			}

			block = buddy_next_buddy(buddy);
			if (block < buddy_allocator->tail) {
				buddy = buddy_next_buddy(block);
			}
			continue;
		}

		if (block->is_free && size <= block->size && (best_block == NULL || block->size <= best_block->size)) {
			best_block = block;
		}

		if (buddy->is_free && size <= buddy->size && (best_block == NULL || buddy->size < best_block->size)) {
			best_block = buddy;
		}

		if (block->size <= buddy->size) {
			block = buddy_next_buddy(buddy);
			if (block < buddy_allocator->tail) {
				buddy = buddy_next_buddy(block);
			}
		} else {
			block = buddy;
			buddy = buddy_next_buddy(buddy);
		}
	}

	if (best_block != NULL) {
		return buddy_block_split(best_block, size);
	}

	return NULL;
}

void* buddy_allocate(struct buddy_allocator* buddy_allocator, size_t size) {
	if (size != 0) {
		size_t required_size = buddy_required_size(buddy_allocator, size);

		struct buddy_block* found = buddy_find_best_block(buddy_allocator, required_size);
		if (found == NULL) {
			buddy_coalescence(buddy_allocator);
			found = buddy_find_best_block(buddy_allocator, required_size);
		}

		if (found != NULL) {
			found->is_free = false;
			return (void*)((char*)found + buddy_allocator->alignment);
		}
	}

	return NULL;
}

void buddy_allocator_free(struct buddy_allocator* buddy_allocator, void* data) {
	if (data != NULL) {
		struct buddy_block* block;

		if (buddy_allocator->head <= data && data < buddy_allocator->tail) {
			block = (struct buddy_block*)data - buddy_allocator->alignment;
			block->is_free = true;
		}
	}
}
