#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct buddy_block {
	size_t size;
	bool is_free;
};

struct buddy_block* head;
struct buddy_block* tail;

extern uint32_t end_kernel;

#define ALIGNMENT 4096

struct buddy_block* buddy_next_buddy(struct buddy_block* block) {
	return (struct buddy_block*)((char*)block + block->size);
}

void initialize_buddy_allocator() {
	head = (void*)(end_kernel + (ALIGNMENT - (end_kernel % ALIGNMENT)));

	head->size = 1073741824;
	head->is_free = true;
	tail = buddy_next_buddy(head);
}

size_t buddy_required_size(size_t size) {
	size_t actual_size = ALIGNMENT;

	size += sizeof(struct buddy_block);
	size += (ALIGNMENT - (size % ALIGNMENT));

	while (size > actual_size) {
		actual_size <<= 1;
	}
	
	return actual_size;
}


void buddy_coalescence() {
	for (;;) {
		struct buddy_block* block = head;
		struct buddy_block* buddy = buddy_next_buddy(block);

		bool no_coalescence = true;
		while (block < tail && buddy < tail) {
			if (block ->is_free && buddy->is_free && block->size == buddy->size) {
				block->size <<= 1;
				block = buddy_next_buddy(block);
				if (block < tail) {
					buddy = buddy_next_buddy(block);
					no_coalescence = false;
				}
			} else if (block->size < buddy->size) {
				block = buddy;
				buddy = buddy_next_buddy(buddy);
			} else {
				block = buddy_next_buddy(buddy);
				if (block < tail) {
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

struct buddy_block* buddy_find_best_block(size_t size) {
	struct buddy_block* best_block = NULL;
	struct buddy_block* block = head;
	struct buddy_block* buddy = buddy_next_buddy(block);

	if (buddy == tail && block->is_free) {
		return buddy_block_split(block, size);
	}

	while (block < tail && buddy < tail) {
		if (block->is_free && buddy->is_free && block->size == buddy->size) {
			block->size <<= 1;
			if (size <= block->size && (best_block == NULL || block->size <= best_block->size)) {
				best_block = block;
			}

			block = buddy_next_buddy(buddy);
			if (block < tail) {
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
			if (block < tail) {
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

void* buddy_allocate(size_t size) {
	if (size != 0) {
		size_t required_size = buddy_required_size(size);

		struct buddy_block* found = buddy_find_best_block(required_size);
		if (found == NULL) {
			buddy_coalescence();
			found = buddy_find_best_block(required_size);
		}

		if (found != NULL) {
			found->is_free = false;
			return (void*)((char*)found + ALIGNMENT);
		}
	}

	return NULL;
}

void buddy_allocator_free(void* data) {
	if (data != NULL) {
		struct buddy_block* block;

		if (head <= data && data < tail) {
			block = (struct buddy_block*)data - ALIGNMENT;
			block->is_free = true;
		}
	}
}
